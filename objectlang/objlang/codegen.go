package objlang

import "fmt"
import "errors"
import "io"
import "github.com/maxymania/langtransf/ast"

type VarAllocator map[string]bool
func (v VarAllocator) Alloc(s string) string {
	i := 0
	for {
		k := fmt.Sprintf("v_%s_%d",s,i)
		if v[k] {
			i++
			continue
		}
		v[k] = true
		return k
	}
}

type RegisterAlloc struct{
	M map[string]int
	C int
}
func (r *RegisterAlloc) Init(){
	r.M = make(map[string]int)
}
func (r *RegisterAlloc) Alloc() string{
	g := fmt.Sprintf("r%d",r.C)
	for k,rc := range r.M {
		if rc==0 {
			g = k
			goto done
		}
	}
	r.C++
	done:
	r.M[g]++
	return g
}
func (r *RegisterAlloc) Incr(s string) {
	r.M[s]++
}
func (r *RegisterAlloc) Decr(s string) {
	r.M[s]--
}

type Var struct{
	Name string
	Type string
}

type CompileContext struct{
	V VarAllocator
	R *RegisterAlloc
	Vars map[string]Var
}

func (c *CompileContext) Init(){
	c.V = make(VarAllocator)
	c.R = new(RegisterAlloc)
	c.R.Init()
	c.Vars = make(map[string]Var)
}
func (c *CompileContext) TComb(o1,op,o2 string) (op2 string,r string,e error){
	op2=op
	switch o1{
	case "integer":
		switch o2{
		case "integer","number":
			r = o2
			switch op{
			case "/":
				r = "number"
				return
			case "+","-","*","%":
				return
			}
		}
	case "number":
		switch o2{
		case "integer","number":
			r = "number"
			switch op{
			case "+","-","*","/","%":
				return
			}
		}
	}
	return
}
func (c *CompileContext) Free(s string){
	switch s[0]{
	case 'r':
		c.R.Decr(s)
	}
}
func (c *CompileContext) CompileExpr(dest io.Writer, a []*ast.AST) (tname string,res string,err error) {
	//res = c.R.Alloc()
	for i,bit := range a {
		if i==0 {
			switch bit.Head{
			case "assign":
				v,ok := c.Vars[bit.Childs[0].Content]
				if !ok{
					err = errors.New("var doesnt exist: "+bit.Content)
					return
				}
				tname = v.Type
				res = v.Name
				_,itn,e := c.CompileExpr(dest,bit.Childs[1:])
				if e!=nil { err=e; return }
				fmt.Fprintf(dest, "%s = %s\n",res,itn)
				c.Free(itn)
				continue
			default:
				if bit.Head!="" {
					err = errors.New("unsupported type")
					return
				}
			}
			switch bit.Token.Type() {
			case "Int":
				tname = "integer"
				res = " "+bit.Content
				continue
			case "Ident":
				v,ok := c.Vars[bit.Content]
				if !ok{
					err = errors.New("var doesnt exist: "+bit.Content)
					return
				}
				tname = v.Type
				res = v.Name
				continue
			}
			err = errors.New("unsupported type")
			return
		} else {
			o1 := res
			if res[0]=='r' {
				c.R.Incr(res)
			}else{
				res = c.R.Alloc()
			}
			switch bit.Head {
			case "binop":
				op := bit.Childs[0].Content
				switch op {
				case "+","-","*","/","%":
					o2t,o2,e := c.CompileExpr(dest,bit.Childs[1:])
					if e!=nil { err=e; return }
					nop,rt,e := c.TComb(tname,op,o2t)
					if e!=nil { err=e; return }
					tname=rt
					fmt.Fprintf(dest,"%s = %s %s %s\n",res,o1,nop,o2)
					c.Free(o1)
					c.Free(o2)
				}
			}
		}
	}
	return
}
func (c *CompileContext) CompileStatement(dest io.Writer, a []*ast.AST) (err error) {
	for _,bit := range a {
		switch bit.Head{
		case "decl":
			name := bit.Childs[1].Content
			regist := c.V.Alloc(name)
			vtyp := "integer" // TODO: change
			c.Vars[name] = Var{regist,vtyp}
			if  len(bit.Childs)>2 {
				if bit.Childs[2].Head=="init" {
					_,res,e := c.CompileExpr(dest,bit.Childs[2].Childs)
					if e!=nil { err=e; return }
					fmt.Fprintf(dest,"%s = %s\n",regist,res) // TODO: change -> cast
					c.Free(res)
				}
			}
		case "expr":
			_,res,e := c.CompileExpr(dest,bit.Childs)
			if e!=nil { err=e; return }
			c.Free(res)
		}
	}
	return
}


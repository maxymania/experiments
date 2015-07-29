package objlang

import "fmt"
import "errors"
import "io"
import "github.com/maxymania/langtransf/ast"
import "objlang/tsys"

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

type Var struct{
	Name string
	Type tsys.DTYPE
}

type CompileContext struct{
	V VarAllocator
	R *tsys.RegisterAlloc
	Vars map[string]Var
}

func (c *CompileContext) Init(){
	c.V = make(VarAllocator)
	c.R = new(tsys.RegisterAlloc)
	c.R.Init()
	c.Vars = make(map[string]Var)
}
func (c *CompileContext) CompileExpr(dest io.Writer, a []*ast.AST) (tname tsys.DTYPE,res string,err error) {
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
				ot,itn,e := c.CompileExpr(dest,bit.Childs[1:])
				if e!=nil { err=e; return }
				if tname.Name()!=ot.Name(){
					cst,re := tname.Cast(itn,ot,c.R)
					if re=="" {
						err = errors.New("invalid cast: ("+tname.Name()+")"+ot.Name() )
						return
					}
					fmt.Fprint(dest,cst)
					c.R.Decr(itn)
					itn = re
				}
				fmt.Fprintf(dest, "%s = %s\n",res,itn)
				c.R.Decr(itn)
				continue
			default:
				if bit.Head!="" {
					err = errors.New("unsupported type")
					return
				}
			}
			switch bit.Token.Type() {
			case "Int":
				tname = tsys.FIXTYPES["integer"]
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
			switch bit.Head {
			case "binop":
				op := bit.Childs[0].Content
				switch op {
				case "+","-","*","/","%":
					o2t,o2,e := c.CompileExpr(dest,bit.Childs[1:])
					if e!=nil { err=e; return }
					code,rg,rt := tname.Binop(o2t,op,res,o2,c.R)
					if rt==nil {
						err = errors.New("invalid op: "+tname.Name()+op+o2t.Name())
						return
					}
					fmt.Fprint(dest,code)
					c.R.Decr(res)
					c.R.Decr(o2)
					res = rg
					tname = rt
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
			vtyp := tsys.FIXTYPES["integer"] // TODO: change
			c.Vars[name] = Var{regist,vtyp}
			if  len(bit.Childs)>2 {
				if bit.Childs[2].Head=="init" {
					_,res,e := c.CompileExpr(dest,bit.Childs[2].Childs)
					if e!=nil { err=e; return }
					fmt.Fprintf(dest,"%s = %s\n",regist,res) // TODO: change -> cast
					c.R.Decr(res)
				}
			}
		case "expr":
			_,res,e := c.CompileExpr(dest,bit.Childs)
			if e!=nil { err=e; return }
			c.R.Decr(res)
		}
	}
	return
}


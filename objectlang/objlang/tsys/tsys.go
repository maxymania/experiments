package tsys

import "fmt"

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
	if len(s)>0 && s[0]=='r' {
		r.M[s]++
	}
}
func (r *RegisterAlloc) Decr(s string) {
	if len(s)>0 && s[0]=='r' {
		r.M[s]--
	}
}

type DTYPE interface{
	Name() string
	Binop(o DTYPE,op,a1,a2 string, al *RegisterAlloc) (code, reg string,rt DTYPE)
	Unop(op,a string, al *RegisterAlloc) (code, reg string,rt DTYPE)
	Cast(a string,rt DTYPE, al *RegisterAlloc) (code, reg string)
}
type nooptype struct{}
func (n nooptype) Name() string { return "" }
func (n nooptype) Binop(o DTYPE,op,a1,a2 string, al *RegisterAlloc) (code, reg string,rt DTYPE) { return }
func (n nooptype) Unop(op,a string, al *RegisterAlloc) (code, reg string,rt DTYPE) { return }
func (n nooptype) Cast(a string,rt DTYPE, al *RegisterAlloc) (code, reg string) { return }

var FIXTYPES = map[string]DTYPE{
	"any"     : tany{},
	"number"  : tnumber{i:false},
	"integer" : tnumber{i:true},
	"boolean" : tboolean{},
}


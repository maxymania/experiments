package tsys

import "fmt"

type tany struct{
	nooptype
}
func (t tany) Name() string { return "any" }

type tnumber struct{
	nooptype
	i bool
}
func (t tnumber) Name() string { if t.i { return "number" } else { return "integer" } }
func (t tnumber) Binop(o DTYPE,op,a1,a2 string, al *RegisterAlloc) (code, reg string,rt DTYPE) {
	ot,ok := o.(tnumber)
	if !ok { return }
	rt = tnumber{i:t.i && ot.i && op!="/"}
	reg = al.Alloc()
	code = fmt.Sprintf("%s = %s %s %s\n",reg,a1,op,a2)
	return
}
func (t tnumber) Unop(op,a string, al *RegisterAlloc) (code, reg string,rt DTYPE) {
	switch op {
	case "-":
		reg = al.Alloc()
		code = reg+" = - "+a+"\n"
		rt=t
	}
	return
}
func (t tnumber) Cast(a string,rt DTYPE, al *RegisterAlloc) (code, reg string) {
	if ot,ok := rt.(tnumber) ; ok{
		if ot.i || !t.i {
			al.Incr(a)
			return "",a
		}
	}
	reg = al.Alloc()
	if t.i {
		code = fmt.Sprintf("%s = rt.cast_integer(%s)\n",reg,a)
	}else{
		code = fmt.Sprintf("%s = rt.cast_number(%s)\n",reg,a)
	}
	return
}

type tboolean struct{
	nooptype
}
func (t tboolean) Name() string { return "boolean" }
//func (t tboolean) Binop(o DTYPE,op,a1,a2 string, al *RegisterAlloc) (code, reg string,rt DTYPE) {
//	ot,ok := o.(tnumber)
//	if !ok { return }
//	code = fmt.Sprintf("%s = %s %s %s\n",reg,a1,op,a2)
//	return
//}
func (t tboolean) Unop(op,a string, al *RegisterAlloc) (code, reg string,rt DTYPE) {
	switch op {
	case "!":
		reg = al.Alloc()
		code = reg+" = not "+a+"\n"
		rt=t
	}
	return
}
func (t tboolean) Cast(a string,rt DTYPE, al *RegisterAlloc) (code, reg string) {
	reg = al.Alloc()
	code = fmt.Sprintf("%s = rt.cast_boolean(%s)\n",reg,a)
	return
}



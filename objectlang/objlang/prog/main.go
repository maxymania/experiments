package main

import "objlang"
//import "objlang/tsys"
import "fmt"
import "os"

func main(){
	//fobj,_ := os.Open(`D:\developement\github\experiments\objectlang\examples\firststeps.ola`)
	//fobj,_ := os.Open(`/home/simon/devel/languages/objectlang/examples/firststeps.ola`)
	fobj,_ := os.Open(`/home/simon/devel/go/mylang/experiments/objectlang/examples/firststeps.ola`)
	defer fobj.Close()
	a,e,ok := objlang.Parse(fobj)
	fmt.Println(a,e,ok)
	occ := new(objlang.CompileContext)
	occ.Init()
	//occ.Vars["i"]=objlang.Var{"v_i_0",tsys.FIXTYPES["integer"]}
	//occ.Vars["j"]=objlang.Var{"v_j_0",tsys.FIXTYPES["integer"]}
	fmt.Println(occ.CompileStatement(os.Stdout,a.Childs))
	fmt.Println(occ.R)
	//os.Stdout.Flush()
}


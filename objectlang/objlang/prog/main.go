package main

import "objlang"
import "fmt"
import "os"

func main(){
	fobj,_ := os.Open(`D:\developement\github\experiments\objectlang\examples\firststeps.ola`)
	//fobj,_ := os.Open(`/home/simon/devel/languages/objectlang/examples/firststeps.ola`)
	defer fobj.Close()
	a,e,ok := objlang.Parse(fobj)
	fmt.Println(a,e,ok)
	occ := new(objlang.CompileContext)
	occ.Init()
	occ.Vars["i"]=objlang.Var{"v_i_0","integer"}
	occ.Vars["j"]=objlang.Var{"v_j_0","integer"}
	fmt.Println(occ.CompileStatement(os.Stdout,a.Childs))
	fmt.Println(occ.R)
	//os.Stdout.Flush()
}


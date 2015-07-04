package main

import "objlang"
import "fmt"
import "os"

func main(){
	fobj,_ := os.Open("/home/simon/devel/languages/objectlang/examples/firststeps.ola")
	defer fobj.Close()
	a,e,ok := objlang.Parse(fobj)
	fmt.Println(a,e,ok)
}


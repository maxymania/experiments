package objlang

import "io"
import "strings"
import "github.com/maxymania/langtransf/syntaxfile"
import "github.com/maxymania/langtransf/ast"
import "github.com/maxymania/langtransf/parsing"

const syntax = `

expr1	= Int|Ident;
expr1x	= ~("binop" ("*"|"/"|"%") expr1 );
expr2	= expr1 expr1x*;
expr2x	= ~("binop" ("+"|"-") expr2 );

expr	= expr2 expr2x*;

//statement	= ("{" statement* #e "}" #E ) #p;

`

var syntax_obj = syntaxfile.Parse(strings.NewReader(syntax))
var syntax_km = make(map[string]string)

func init(){
	syntax_obj.ScanForKeyWords(syntax_km)
}

func Parse(src io.Reader) (a *ast.AST,e *syntaxfile.ErrorRecorder,ok bool){
	token := new(parsing.StdScanner).Init(syntax_km,src).FirstToken()
	a = new(ast.AST)
	e = new(syntaxfile.ErrorRecorder)
	ok = nil!=syntax_obj["expr"].Parse(syntax_obj,token,a,e)
	return
}

// Ukazka prace s retezci a vestavenymi funkcemi 

write("Ahoj\n\"Sve'te \\\u{0022}", "\n")  // 042 oktalove = 34 decimalne = 22 hexadecimalne = znak uvozovek

var s1 : String? = 
  """
   Toto 
   je 

   nejaky 
   text
 """  // ukoncujici uvozovky ovlivnuji implicitni odsazeni vnitrnich radku retezce
var s2:String = "a"//(s1!) + ", ktery jeste trochu obohatime"
write(s1, "\u{000020}", s2, "\u{0A}")

var s1String ="1"// s1!
var s1len = length(s1String)
var s1_20ty_znak ="1"// s1len
s1len = 4
let s1_16ty_znak : Int = 1//s1_20ty_znak - s1len
let s11 = substring(of: s2, startingAt: s1_16ty_znak, endingBefore: s1_20ty_znak)
write(s1len, " znaky od ", 16, ". znaku (index ", s1_16ty_znak, ") v \"", s2, "\":", s11, "\n")

let s3 = substring(of:s2,startingAt:15,endingBefore:19) 
s1String = "1"//s3 ?? 
// """
// viceradkovy
// "retezec"
// misto nil vysledku
// """ // vysledny retezec v s1 bude mit 4 znaky
let s1len4 = length(s1String)
write(s1len4, " znaky od ", 16, ". znaku v \"", s2, "\":", s1, "\n")

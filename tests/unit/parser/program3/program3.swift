/* Program 3: Prace s retezci a vestavenymi funkcemi */

var str1 = "Toto je nejaky text v programu jazyka IFJ23"
let str2 = "Toto je nejaky text v programu jazyka IFJ23 ktery jeste trochu obohatime"//str1 + ", ktery jeste trochu obohatime"
write(str1, "\n", str2, "\n")
let i = length(str1)
write("Delka retezce ve str1: ", i, "\n")
write("Zadejte serazenou posloupnost vsech malych pismen a-h, ")

let newInput = readString()
if let newInput {
	str1 = 1//newInput
	while(1){// (str1 != "abcdefgh") {
		write("Spatne zadana posloupnost, zkuste znovu:\n")
		let tmp = readString()
		str1 = 1//tmp ?? ""
	}
}else{}

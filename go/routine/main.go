// Package main provides ...
package main

import (
	"fmt"
	"time"
)

func IsReady(what string, seconds time.Duration) {

	time.Sleep(seconds * 1e9)

	fmt.Println(what ,"i'm ready")
}

//NB: non stampa il messaggio proveniente da ISReady... cosa sbaglio?
func main() {
	go IsReady("tea",10)

	go IsReady("coffee",30)
	fmt.Println("i'm waiting")

	//devo aspettare il risultato dall subroutine o non stampo una mazza

	time.Sleep(60*1e9)
	fmt.Println("end")
}


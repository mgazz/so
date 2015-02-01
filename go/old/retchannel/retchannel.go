// Package main provides ...
package main

import (
	"log"
)

//creo e il canale nella funzione partenza e lo ritorno
func partenza() chan int {
	ch:= make( chan int)
	go func() {
		for i := 0; ; i++ {
			ch<-i
		}
	}()
	return ch
}
func main() {
	//stream ora è il canale da cui leggo il primo messaggio
	stream:= partenza()
	log.Println("stampa: ",<-stream)
}

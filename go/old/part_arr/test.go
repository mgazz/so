// Package main provides ...
package main
import (
	"fmt"
)

func partenza(ch chan<- int) {
	fmt.Println("partenza")
	for i := 0;i<10 ; i++ { 
		fmt.Println("invio messaggio")	
		ch <- i } // invia!
	}
func arrivo(ch <-chan int) {
	for { 
		fmt.Println("ricevo messaggio")
		fmt.Println(<-ch)
	} // ricevi e stampa!
}
func main() {
	log.Println("debug")
	ch1 := make(chan int)
	go partenza(ch1) 
	go arrivo(ch1)
	
		
	
}

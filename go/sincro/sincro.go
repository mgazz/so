// Package main provides ...
package main
import (
	"time"
	"log"
)

func figlio(ch chan bool) {
	time.Sleep(10*time.Second)
	ch<-true
}
func main() {
	ch:= make(chan bool)
	log.Println("lancio figlio")
	go figlio(ch)
	log.Println("aspetto figlio")
	<-ch//attesa figlio
	log.Println("ritornato")
}

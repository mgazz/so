// Package main provides ...
package main

import (
	"fmt"
	"time"
)

//NB: default i canali non sono bufferizzati e comunicazione sincrona bloccante
//NB: la receive da un canle non inizializzato (nil) is bloccante

//mod inserimento nel canale
func partenza(ch chan<-int) {

	for i := 0; ; i++ {
		ch <-i //invia stampa
		time.Sleep(1e9)
	}

}

// mod estraz dal canale
func arrivo(ch <-chan int) {

	for i := 0; ; i++ {
		fmt.Println("ricevuto: ", <-ch) //ricevi stampa
	}

}


func main() {

	var ch chan int
	ch = make(chan int )

	go partenza(ch)
	go arrivo(ch)

	time.Sleep(5*1e9)

	fmt.Println("end!");

}


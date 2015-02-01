// Package main provides ...
package main

import (
	"fmt"
	"time"
)
const (
	cons int = 3
	prod int = 3
	max_proc int = 10
	max_proc int 
)
var pronto_cons = make(chan int )
var pronto_prod = make(chan int )
var dati = make(chan int)
var dati_cons[max_proc]chan int
var contatore int = 0

func when(b bool, c chan int) chan int {
	if b==false {
		return nil
	} else {
		return c
	}
}

func select() {
	select {
	case x := <-when(contatore<N,pronto_prod)
		contatore++
		msg = <-dati //ricezione messaggio da inserire
		//inserimento messaggio in buffer
	case x := <-when(contatore>0,pronto_cons)
		contatore--
		//estrazione dal buffer
		da_cons[x]<-msg //consegna consumatore
	}
}

func main() {
	fmt.Println("start main!");
	fmt.Printf("inserisci il numero dei processi\n")
	fmt.Scanf("%d/n",&n_proc)

	//inizializzo il canale per i processi 
	for i := 0; i < n_proc; i++ {
		dati_cons[i] = make(chan int,n_proc)
	}
}


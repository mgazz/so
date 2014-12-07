
// buffer_concorrente project main.go
package main

import (
	"fmt"
	"time"
)

const MAXPROD = 10
const MAXCONS = 10
const MAXBUFF int = 5 // MAX capacitÃ  buffer

var dimbuf int //capacitÃ  effettiva
var prod int
var cons int
var done = make(chan bool)
var buf_in [MAXBUFF]chan int  // canali comunicazione elementi
var termina [MAXBUFF]chan int // per la terminazione elementi
var DATI_CONS = make(chan int)
var pronto_cons = make(chan int)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}

func producer(myid int, pronto_prod chan int) {
	for i := 0; i < 10; i++ {
		pronto_prod <- i + ((myid) * 10)
		fmt.Printf("[producer%d]  ho scritto:%d\n ", myid, i+((myid)*10))
		time.Sleep(1e9)
	}
	done <- true
	return
}

func consumer(myid int) {
	fmt.Printf("[consumer%d] eccomi! \n", myid)
	for {
		msg, ok := <-DATI_CONS
		if !ok {
			fmt.Printf("[consumer%d]  FINE.\n", myid)
			done <- true
			return
		}
		fmt.Printf("[consumer%d]  ho letto: %d\n", myid, msg)
	}

}

func server(stage int, ch_in chan int, ch_out chan int) {

	if stage != dimbuf-1 { //stage intermedio
		for {
			//time.Sleep(100)
			select {
			case x := <-ch_in:
				fmt.Printf("[stage %d]  ricevuto valore: %d  \n", stage, x)
				ch_out <- x
			case <-termina[stage]:
				termina[stage+1] <- 1 //propagazione al successivo
				fmt.Printf("[buffer]  TERMINO! \n")
				done <- true
				return
			}
		}

	} else { //ultimo stage
		for {
			//time.Sleep(100)
			select {
			case msg := <-ch_in:
				fmt.Printf("[stage %d]  ricevuto valore: %d  \n", stage, msg)
				DATI_CONS <- msg
				//fmt.Printf("[buffer]  prelevato: %d da consumatore %d \n", msg, x)

			case <-termina[stage]:
				fmt.Printf("[buffer]  TERMINO! \n")
				close(DATI_CONS)

				done <- true
				return
			}
		}
	}
}

func main() {

	fmt.Printf("\n quanti produttori (max %d)? ", MAXPROD)
	fmt.Scanf("%d", &prod)
	fmt.Printf("\n quanti consumatori(max %d) ? ", MAXCONS)
	fmt.Scanf("%d", &cons)
	fmt.Println("ecco cons:", cons)
	fmt.Printf("\n dimensione buffer (max %d) ? ", MAXBUFF)
	fmt.Scanf("%d", &dimbuf)
	//inizializzazione canali
	for i := 0; i < dimbuf; i++ {
		buf_in[i] = make(chan int)
	}
	for i := 0; i < dimbuf; i++ {
		termina[i] = make(chan int)
	}

	// creazione stages:
	for i := 0; i < dimbuf-1; i++ {
		go server(i, buf_in[i], buf_in[i+1])
	}
	go server(dimbuf-1, buf_in[dimbuf-1], nil) // creazione ultimo stage

	//creazione produttori
	for i := 0; i < prod; i++ {
		go producer(i, buf_in[0])
	}

	for i := 0; i < cons; i++ {
		go consumer(i)
	}
	// attesa produttori
	for i := 0; i < prod; i++ {
		<-done
	}
	//terminazione server
	termina[0] <- 1
	// attesa server
	for i := 0; i < dimbuf; i++ {
		<-done
	}

	//attesa consumatori
	for i := 0; i < cons; i++ {
		<-done
	}
}


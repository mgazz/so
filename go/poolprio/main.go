
// resourcepool_prio project main.go

package main

import (
	"fmt"
	"time"
)

const MAXPROC = 10
const MAXRES = 3
const MAXBUFF = 20

//canali
var richiesta = make(chan int, MAXBUFF)
var rilascio = make(chan int, MAXBUFF)
var risorsa [MAXPROC]chan int
var done = make(chan int)
var termina = make(chan int)

func client(i int) {

	richiesta <- i
	r := <-risorsa[i]
	fmt.Printf("\n [client %d] uso della risorsa %d\n", i, r)
	time.Sleep(time.Second * 2)
	//rilascio la risorsa
	rilascio <- r
	//comunico di aver finito
	done <- i
}

func server() {
	var disponibili int = MAXRES
	var res, p, i int
	var libera [MAXRES]bool
	var sospesi = 0
	var bloccato [MAXPROC]bool

	// inizializzazioni:
	//megli non mi devo preoccupare del segmentation fault
	for i :=  range libera {
		libera[i] = true
	}
	for i := 0; i < MAXPROC; i++ {
		bloccato[i] = false
	}

	for {
		time.Sleep(time.Second * 1)
		fmt.Println("nuovo ciclo server")
		select {
		// caso in cui un processo ha finito e rilascia la risorsa
		case res = <-rilascio:
			if sospesi == 0 {
				//se nessuno sospeso aumento le risorse disponibili
				disponibili++
				libera[res] = true
				fmt.Printf("[server]  restituita risorsa: %d  \n", res)
			} else {
				// se no ciclo dall'indice maggiore finchè nn trovo un processo bloccato
				for i = MAXPROC - 1; i >= 0 && !bloccato[i]; i-- { //prio agli indici maggiori
				}
				//bloccato[i] è il processo bloccato di indice maggiore
				//lo sblocco ed essendo sbloccato diminuisco il numero dei processi sospesi
				bloccato[i] = false
				sospesi--

				risorsa[i] <- res
				fmt.Printf("[server]  risvegliato processo %d - allocazione risorsa %d  \n", i, res)
			}

		case p = <-richiesta:
			if disponibili > 0 {
				for i = 0; i < MAXRES && !libera[i]; i++ {
				}
				libera[i] = false
				disponibili--
				risorsa[p] <- i
				fmt.Printf("[server]  allocata risorsa %d a cliente %d \n", i, p)
			} else {
				sospesi++
				fmt.Printf("[server]  cliente %d in attesa .. \n", p)
				bloccato[p] = true
			}
		case <-termina: // quando tutti i processi hanno finito
			fmt.Println("FINE !!!!!!")
			done <- 1
			return

		}
	}
}

func main() {
	var cli int
	fmt.Printf("\n quanti clienti (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &cli)
	fmt.Println("clienti:", cli)

	//inizializzazione canali
	for i := 0; i < cli; i++ {
		risorsa[i] = make(chan int, MAXBUFF)
	}

	for i := 0; i < cli; i++ {
		go client(i)
	}
	go server()

	for i := 0; i < cli; i++ {
		<-done
	}
	termina <- 1
	<-done //attesa terminazione server

}

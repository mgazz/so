package main

import (
	"fmt"
	"time"
)

//risorsa
//client
//server
const (
	buff_size = 20
	n_risorsa = 3
	max_client = 10
)

var client_channel[max_client] chan int // risorsa disponibili
var req_channel= make(chan int ,buff_size)	//request resource
var rel_channel= make(chan int ,buff_size)	 //release resource
var stop= make(chan int )	//used by main to force termination of routines
var done= make(chan int )	//used by main to wait all routines

func client(id_client int) {
	//faccio una richiesta --> server mi risponde 
	req_channel<-id_client
	//in r ho l'id della risorsa ricevuta
	id_risorsa := <-client_channel[id_client]
	fmt.Printf("\t[client %d] ricevuto risorsa [%d]\n", id_client, id_risorsa)
	time.Sleep(2*time.Second)
	fmt.Printf("\t[client %d] libera risorsa [%d]\n", id_client, id_risorsa)
	rel_channel <- id_risorsa
	done <-id_client
}

func server() {
	var i int
	var disp_risorsa int = n_risorsa
	var n_block_client int
	var free_risorsa[n_risorsa] bool
	var bloc_client[max_client] bool

	//inizializzazione

	//in partenza tutte le risorsa sono libere
	for i :=  range free_risorsa {
		free_risorsa[i]=true
	}
	// in partenza nessun client bloccato
	for i := 0; i < max_client; i++ {
		bloc_client[i]=false
	}

	//partenza server
	fmt.Printf("---start server---\n")
	for  {
		//time.Sleep(2*time.Second)
		//guardie
		select {
		//arrivo richiesta con id = idclient
		case id_client := <-req_channel:
			if disp_risorsa > 0 {
				//esco alla prima risorsa libera==true
				for i = 0; i < n_risorsa && free_risorsa[i] == false; i++ {
					//risorsa non libera
				}
				//ho la sicurezza che esce prima di aver ciclato tutte le risorse --> no overflow
				disp_risorsa--
				free_risorsa[i]=false
				//comunico al client in comunicazione sul canale numero request che usa risorsa i
				fmt.Printf("server | client [%d] viene assegnata risorsa [%d]\n",id_client,i)
				client_channel[id_client] <-i
			} else {
				bloc_client[id_client]=true
				n_block_client++
				fmt.Printf("server | client [%d] messo in attesa\n", id_client)
			}

		case id_risorsa := <-rel_channel:
			if n_block_client > 0 {
				// esco al primo client bloccato == true
				for i = max_client-1; i>=0 && bloc_client[i]==false; i-- {
				}
				n_block_client--
				bloc_client[i]=false
				fmt.Printf("server | riassegno risorsa [%d] passa al client [%d]\n", id_risorsa,i)
				// al client i bloccato assegno la risorsa liberata
				client_channel[i] <- id_risorsa
			} else {
				free_risorsa[id_risorsa]=true
				disp_risorsa++
				fmt.Printf("server | no client in attesa -> risorsa [%d] liberata\n",id_risorsa)
			}
		case <-stop:
			fmt.Printf("sever termina!\n")
			done <- 1
			return
		}
	}
}

func main() {
	var n_proc int
	fmt.Printf("inserisci il numero dei processi\n")
	fmt.Scanf("%d/n",&n_proc)

	//inizializzo il numero di processi
	for i := 0; i < n_proc; i++ {
		client_channel[i] = make(chan int ,buff_size)
	}

	fmt.Printf("main | start programma\n")
	//start client
	for i := 0; i < n_proc; i++ {
		go client(i)
	}
	//start server
	go server()

	// aspetto terminazione client
	for i := 0; i < n_proc; i++ {
		id_client:=<-done
		fmt.Printf("main| client [%d] terminato\n", id_client)
	}
	//termino server
	stop<-1
	//aspetto terminazione del server
	<- done
	fmt.Printf("main | fine programma\n")
}



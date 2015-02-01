// Package main provides ...
package main

import (
	"fmt"
	"time"
)
const (
	max_proc = 10
	n_res = 3
	max_buffer = 20
)
var req_chan = make(chan int ,max_proc)
var rel_chan = make(chan int ,max_proc)
var done = make(chan int)
var end = make(chan bool)


var global_channel[max_proc] chan int

func client(id int) {
	fmt.Printf("client [%d] - start\n", id)
	req_chan <- id
	id_risorsa := <-global_channel[id]
	fmt.Printf("client [%d] - ricevuta risorsa [%d]\n", id,id_risorsa)
	time.Sleep(2*time.Second)
	fmt.Printf("client [%d] - libera la risorsa [%d]\n", id,id_risorsa)
	rel_chan <- id_risorsa
	fmt.Printf("client [%d] - comunica terminazione\n", id)
	done <- id
}

func server() {
	//var ris_disp int = n_res
	//var blocked_client int = 0
	var coda_client []int
	var coda_ris_disp [] int

	//creo una coda delle risorse disponibili
	for i := 0; i < n_res; i++ {
		coda_ris_disp=append(coda_ris_disp,i)
	}
	fmt.Printf("coda_ris_disp:%d\n", coda_ris_disp)
	
	fmt.Printf("====== server start ======\n")
	for  {
		select {
		case id_cliente := <-req_chan:
			if len(coda_ris_disp)>0 {
			fmt.Printf("server | assegno risorsa [%d] - al client [%d]\n", coda_ris_disp[0],id_cliente)
				global_channel[id_cliente] <- coda_ris_disp[0]
				coda_ris_disp = coda_ris_disp[1:]
				//ris_disp--
				fmt.Printf("coda_ris_disp: %d\n", coda_ris_disp)
			} else {
				fmt.Printf("server | risorse non disponibili -> cliente [%d] in coda\n", id_cliente)
				coda_client = append(coda_client,id_cliente)
				fmt.Printf("coda_client:%d\n", coda_client)
				//blocked_client++
			}
		case id_risorsa:= <-rel_chan:
			if len(coda_client)>0 {
				fmt.Printf("server | risorsa [%d] liberata ma clienti in coda:\n", id_risorsa)
				fmt.Printf("coda_client:%d\n", coda_client)
				next_cliente_servito := coda_client[0]
				coda_client = coda_client[1:]
				//blocked_client--
				global_channel[next_cliente_servito] <- id_risorsa
			} else {
				fmt.Printf("server | nessun cliente in coda, aggiungo risorsa [%d] a risorse disponibili\n", id_risorsa)
				coda_ris_disp = append(coda_ris_disp,id_risorsa)
				//ris_disp++
				fmt.Printf("coda_ris_disp:%d\n", coda_ris_disp)
			}
		case <-end:
			fmt.Printf("====== server termina ======\n")
			done<-0
		}
	}
}

func main() {

	var n_proc int
	fmt.Println("====== main	start	======");
	fmt.Printf("inserisci il numero dei processi (max %d):\n",max_proc)
	fmt.Scanf("%d",&n_proc)
	
	for i := 0; i < n_proc; i++ {
		global_channel[i] = make(chan int)
	}
	for i := 0; i < n_proc; i++ {
		go client(i)
	}
	go server()

	for i := 0; i < n_proc; i++ {
		id_cliente := <-done
		fmt.Printf("client [%d] - terminato\n", id_cliente)
	}

	fmt.Printf("comunico stop server\n")

	end<-true
	fmt.Printf("aspetto terminazione server\n")
	<-done
	fmt.Printf("server terminato\n")
}


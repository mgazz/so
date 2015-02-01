
// resourcepool project main.go
package main

import (
	"fmt"
	"time"
	//"sort"
)

const MAXPROC = 50
const MAXRES = 5

var richiesta = make(chan int)
var rilascio = make(chan int)
var risorsa [MAXPROC]chan int
var done = make(chan int)
var termina = make(chan int)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}

func client(i int) {

	richiesta <- i
	r := <-risorsa[i]
	fmt.Printf("\n [client %d] uso della risorsa %d\n", i, r)
	rilascio <- r
	done <- i
}

func server() {

	//var disponibili int = MAXRES
	var res, p, i int
	var libera []int

	for i := 0; i < MAXRES; i++ {
		libera=append(libera,i)
	}

	for {
		time.Sleep(time.Second * 1)
		fmt.Println("nuovo ciclo server")
		select {
		case res = <-rilascio:
			libera=append(libera,res)
			//sort.Sort(sort.IntSlice(libera))
			fmt.Printf("[server]  restituita risorsa: %d  \n", res)

		case p = <-when(len(libera)>0, richiesta):
			i=libera[0]
			libera=libera[1:]
			risorsa[p] <- i
			fmt.Printf("[server]  allocata risorsa %d a cliente %d \n", i, p)
		case <-termina: // quando tutti i processi clienti hanno finito
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
	start := time.Now()
	for i := 0; i < MAXPROC; i++ {
		risorsa[i] = make(chan int)
	}

	for i := 0; i < cli; i++ {
		go client(i)
	}
	go server()

	for i := 0; i < cli; i++ {
		<-done
	}
	termina <- 1 //terminazione server
	<-done
	av_time :=time.Now().Sub(start)
	fmt.Println("end - time:",av_time)
}

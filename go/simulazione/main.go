package main

import (
	"fmt"
	"math/rand"
	//"time"
)

const (
	MAXPROC= 10
	MAXBUFF= 10
	MAX = 2
	S=0 //corsia sud
	N=1 //corsia nord
	NS=0 //nord -> sud
	SN=1 //sud -> nord
	I=0 //input
	O=1 //output
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------




//-------------------------------------



var done = make(chan bool)
var termina_ponte = make(chan bool)
var ack_macchina[MAXPROC] chan int
var req_entrata[2]chan int 
var req_uscita[2][2]chan int //[ponte][direzione]


var nmacchine int //numero di client


func macchina(id int) {
	var entrata int
	entrata = rand.Intn(2) //da dove chiede di entrare
	fmt.Printf("macchina %d | va da: %d\n", id,entrata)
	req_entrata[entrata]<-id
	ponte:=<-ack_macchina[id]
	fmt.Printf("client %d | ponte: %d dir:%d\n", id,ponte,entrata)
	req_uscita[ponte][entrata]<-id
	<-ack_macchina[id]

	done <- true
	fmt.Printf("macchina %d | esce dal sistema\n", id)
}


func ponte() {
	fmt.Printf("ponte # | avviato\n")
	var n_su_ponte_nord[2] int
	for i := 0; i < 2; i++ {
		n_su_ponte_nord[i]=0
	}
	var n_su_ponte_sud int=0
	for  {
		//linea di stampa stato
		fmt.Printf("ponte # |\t") 
		for i := 0; i < 2; i++ {
			fmt.Printf("n_su_ponte_nord[%d]: %d\t",i,n_su_ponte_nord[i]) //verso nord, sud
		}
		fmt.Printf("n_su_ponte_sud: %d\t",n_su_ponte_sud) //solo verso sud
		fmt.Printf("\n")
		select {
		case x:= <-when(n_su_ponte_nord[NS]==0 && n_su_ponte_nord[SN]<MAX,req_entrata[SN]): //richiede di entrare da sud
			n_su_ponte_nord[SN]++
			ack_macchina[x] <- N  //ritorno il canale dal quale uscire
		
		case x:= <-when(n_su_ponte_sud<MAX || (n_su_ponte_nord[SN]==0 && n_su_ponte_nord[NS]<MAX),req_entrata[NS]): //richiede di entrare verso nord
		if n_su_ponte_sud<MAX {
			n_su_ponte_sud++
			ack_macchina[x] <- S
		} else {
			n_su_ponte_nord[NS]++
			ack_macchina[x] <- S
		}
			
		case x:= <-when(true,req_uscita[S][NS]): //richiede di uscire da ponte sud in direzione nord->sud
			n_su_ponte_sud--
			ack_macchina[x] <- 1 
		
		case x:= <-when(true,req_uscita[N][SN]):
			n_su_ponte_nord[SN]--
			ack_macchina[x] <- 1

		case x:= <-when(true,req_uscita[N][NS]):
			n_su_ponte_nord[NS]--
			ack_macchina[x] <- 1

		case <-termina_ponte: //terminazione server
			fmt.Printf("ponte # | termina\n")
			done <- true
			return
		}
	}
}


//canali

func init_chan() {
	for i := 0; i < 2; i++ { 
			req_entrata[i]= make(chan int,MAXBUFF) //se prio -> buf
	}
	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			req_uscita[i][j]= make(chan int,MAXBUFF) //se prio -> buf
		}
	}




	for i := 0; i < nmacchine; i++ { //canali ack per macchina
			ack_macchina[i]= make(chan int,MAXBUFF) 
	}
}

func print_chan_len() {
	for i := 0; i < 2; i++ { 
			fmt.Printf("req_entrata[%d]: %d\t",i,len(req_entrata[i]))
	}

	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			fmt.Printf("req_uscita[%d][%d]: %d\t",i,j,len(req_uscita[i][j]))
		}
	}
	
	
}

func main() {

	fmt.Printf("inserisci nmacchine (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nmacchine)

	init_chan()

	for i := 0; i < nmacchine; i++ {
		go macchina(i)
	}


	go ponte()

	//aspetta client
	for i := 0; i < nmacchine; i++ {
		<-done
	}
	//aspetta ponte 
	termina_ponte <- true
	<-done

	fmt.Printf("FINE\n")
}

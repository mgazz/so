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

	MR = 0 //martello
	TN = 1 //tenaglia
	BD = 2 //badile

	NG = 0
	PR = 1
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------
var done = make(chan bool)

var nclienti int //numero di client
var nfornitori int //numero di client

var req_cliente[2][3]chan int 
var ack_cliente[MAXPROC] chan int
var req_fornitore[3]chan int 
var ack_fornitore[MAXPROC] chan int
var termina_magazzino = make(chan bool)


func fornitore(id int) {
	fmt.Printf("fornitore %d | avviato\n", id)
	var esemplare int
	esemplare = rand.Intn(3)
	fmt.Printf("fornitore %d | esemplare: %d\n", id,esemplare)

	req_fornitore[esemplare]<-id
	<-ack_fornitore[id]

	done <- true
	fmt.Printf("fornitore %d | esce dal sistema\n", id)
}

func cliente(id int) {
	fmt.Printf("cliente %d | avviato\n", id)

	var esemplare int
	esemplare = rand.Intn(3)
	var tipo_cliente int
	tipo_cliente= rand.Intn(2)

	fmt.Printf("cliente %d | tipo: %d esemplare: %d\n", id,tipo_cliente,esemplare)

	req_cliente[tipo_cliente][esemplare]<-id
	<-ack_cliente[id]

	done <- true
	fmt.Printf("cliente %d | esce dal sistema\n", id)
}

func negozianti_vuoti() (bool){
	for i := 0; i < 3; i++ {
		if len(req_cliente[NG][i])>0 {
			return false
		}
	}
	return true
}


func magazzino() {
	fmt.Printf("magazzino # | avviato\n")
	var esemplare[3] int
	for i := 0; i < 3; i++ {
		//esemplare[i]=MAX
		esemplare[i]=MAX
	}
	
	for  {
		//linea di stampa stato
		fmt.Printf("magazzino # |\t") 
		for i := 0; i < 3; i++ {
			fmt.Printf("esemplare[%d]: %d\t",i,esemplare[i])
		}
		fmt.Printf("\n")
		select {

		//negozianti
		case x:= <-when(esemplare[BD]>0,req_cliente[NG][BD]): //negoziante badile
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[BD]--
			ack_cliente[x] <- 1
		
		case x:= <-when(esemplare[MR]>0,req_cliente[NG][MR]): //negoziante martello
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[MR]--
			ack_cliente[x] <- 1
			
		case x:= <-when(esemplare[TN]>0,req_cliente[NG][TN]):
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[TN]--
			ack_cliente[x] <- 1

		//privati
		case x:= <-when(esemplare[BD]>0 && negozianti_vuoti()==true,req_cliente[PR][BD]): //negoziante badile
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[BD]--
			ack_cliente[x] <- 1
		
		case x:= <-when(esemplare[MR]>0 && negozianti_vuoti()==true,req_cliente[PR][MR]): //negoziante martello
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[MR]--
			ack_cliente[x] <- 1
			
		case x:= <-when(esemplare[TN]>0 && negozianti_vuoti()==true,req_cliente[PR][TN]):
			//fmt.Printf("cliente[][] %d entrato \n",x )
			esemplare[TN]--
			ack_cliente[x] <- 1

		case x:= <-when(esemplare[MR]==0,req_fornitore[MR]):
			//fmt.Printf("fornitori[MR] %d entrato \n",x )
			esemplare[MR]=MAX
			ack_fornitore[x] <- 1

		case x:= <-when(
			esemplare[MR]==0 && 
			(len(req_fornitore[MR])==0 || 
			(len(req_fornitore[MR])>0 && esemplare[MR]>0)),req_fornitore[TN]):
			//fmt.Printf("fornitori[MR] %d entrato \n",x )
			esemplare[TN]=MAX
			ack_fornitore[x] <- 1
		
		case x:= <-when(
			esemplare[MR]==0 && 
			(len(req_fornitore[MR])==0 && len(req_fornitore[TN])>0) || 
			(len(req_fornitore[MR])>0 && esemplare[MR]>0 && len(req_fornitore[TN])>0 && esemplare[TN]>0 ),req_fornitore[BD]):
			//fmt.Printf("fornitori[MR] %d entrato \n",x )
			esemplare[BD]=MAX
			ack_fornitore[x] <- 1

		case <-termina_magazzino: //terminazione server
			fmt.Printf("magazzino # | termina\n")
			done <- true
			return
		}
	}
}

func init_chan() {
	for i := 0; i < 2; i++ { 
		for j := 0; j < 3; j++ { 
			req_cliente[i][j]= make(chan int,MAXBUFF) //se prio -> buf
		}
	}
	for i := 0; i < nclienti; i++ { //canali ack per cliente
			ack_cliente[i]= make(chan int,MAXBUFF) 
	}
	for i := 0; i < 3; i++ { 
			req_fornitore[i]= make(chan int,MAXBUFF) //se prio -> buf
	}
	for i := 0; i < nfornitori; i++ { //canali ack per fornitore
			ack_fornitore[i]= make(chan int,MAXBUFF) 
	}
}

func print_chan_len() {
	for i := 0; i < 2; i++ { 
		for j := 0; j < 3; j++ { 
			fmt.Printf("req_cliente[%d][%d]: %d\t",i,j,len(req_cliente[i][j]))
		}
	}
	for i := 0; i < 3; i++ { 
			fmt.Printf("req_fornitore[%d]: %d\t",i,len(req_fornitore[i]))
	}
	
}

func main() {
	fmt.Printf("inserisci nclienti (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nclienti)

	fmt.Printf("inserisci nfornitori (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nfornitori)

	init_chan()

	go magazzino()

	for i := 0; i < nfornitori; i++ {
		go fornitore(i)
	}

	for i := 0; i < nclienti; i++ {
		go cliente(i)
	}

	//aspetta fornitori 
	for i := 0; i < nfornitori; i++ {
		<-done
	}

	//aspetta clienti
	for i := 0; i < nclienti; i++ {
		<-done
	}

	//aspetta magazzino 
	termina_magazzino <- true
	<-done

	fmt.Printf("FINE\n")
}

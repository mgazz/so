package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	MAXBUFF= 10
	TI=5
	TS=6
	MAXPROC= TI+TS-1
	PI=0
	PS=1
	I=0
	O=1
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//canali-------

var nmembri int
var nviaggiatori int
var ninps int=0
var ninpi int=0

var done = make(chan bool)

var termina_viaggiatori= make(chan bool)
var termina_membri= make(chan bool)
var termina_scala = make(chan bool)

var req_viaggiatore[2][2]chan int 
var ack_viaggiatore[MAXPROC] chan int

var req_membro[2][2]chan int 
var ack_membro[MAXPROC] chan int

//------------

func scala() {
	fmt.Printf("scala # | avviato\n")
	var ndirps int=0
	var ndirpi int=0
	for  {
		//linea di stampa stato
		fmt.Printf("scala # |\t") 
		//print_chan_len()
		fmt.Printf("ndirpi: %d\t",ndirpi)
		fmt.Printf("ndirps: %d\t",ndirps)
		fmt.Printf("ninpi: %d\t",ninpi)
		fmt.Printf("ninps: %d\t",ninps)
		fmt.Printf("\n")
		select {

		case x:= <-when(ndirpi==0,req_membro[I][PS]):
			//fmt.Printf("viaggiatore[I][PS] %d entrato \n",x )
			ndirps++
			if ninpi>0 {
				ninpi--
			}
			ack_membro[x] <- 1
		
		case x:= <-when(true,req_membro[O][PS]):
			//fmt.Printf("viaggiatore[I][PS] %d entrato \n",x )
			ndirps--
			ninps++
			ack_membro[x] <- 1
		
		case x:= <-when(ndirps==0,req_membro[I][PI]):
			ndirpi++
			if ninps>0 {
				ninps--
			}
			ack_membro[x] <- 1
		
		case x:= <-when(true,req_membro[O][PI]):
			ndirpi--
			ninpi++
			ack_membro[x] <- 1

		case x:= <-when(ndirpi==0 && len(req_membro[I][PI])==0 && len(req_membro[I][PS])==0 && len(req_membro[O][PI])==0 && len(req_membro[O][PS])==0,req_viaggiatore[I][PS]):
			ndirps++
			if ninpi>0 {
				ninpi--
			}
			ack_viaggiatore[x] <- 1
		
		case x:= <-when(ndirps==0 && len(req_membro[I][PI])==0 && len(req_membro[I][PS])==0 && len(req_membro[O][PI])==0 && len(req_membro[O][PS])==0,req_viaggiatore[I][PI]):
			ndirpi++
			if ninps>0 {
				ninps--
			}
			ack_viaggiatore[x] <- 1
		
		case x:= <-when(true,req_viaggiatore[O][PI]):
			ndirpi--
			ninpi++
			ack_viaggiatore[x] <- 1
		
		case x:= <-when(true,req_viaggiatore[O][PS]):
			ndirps--
			ninps++
			ack_viaggiatore[x] <- 1

		case <-termina_scala: //terminazione server
			fmt.Printf("scala # | termina\n")
			done <- true
			return
		}
	}
}


func viaggiatore(id int) {
	fmt.Printf("viaggiatore %d | avviato\n", id)
	var start_position int
	start_position = rand.Intn(2)
	if start_position==PI {
		ninpi++
	} else {
		ninps++
	}
	fmt.Printf("client %d | start_position: %d\n", id,start_position)
	for  {
		if start_position==PI {
			req_viaggiatore[I][PS]<-id //entra nella scala in direzioni PS
			<-ack_viaggiatore[id]
			time.Sleep(time.Duration(rand.Intn(5)) * time.Second)
			req_viaggiatore[O][PS]<-id
			<-ack_viaggiatore[id]
			start_position=PS
		} else {
			req_viaggiatore[I][PI]<-id //entra nella scala in direzioni PI
			<-ack_viaggiatore[id]
			time.Sleep(time.Duration(rand.Intn(5)) * time.Second)
			req_viaggiatore[O][PI]<-id
			<-ack_viaggiatore[id]
			start_position=PI
		}

		select {
		default:

		case <-termina_viaggiatori: //terminazione server
			fmt.Printf("viaggiatore %d | termina\n",id)
			done <- true
			return
		}
	}
}

func membro(id int) {
	var start_position int
	start_position = rand.Intn(2)
	if start_position==PI {
		ninpi++
	} else {
		ninps++
	}
	fmt.Printf("client %d | start_position: %d\n", id,start_position)
	for  {
		if start_position==PI {
			req_membro[I][PS]<-id //entra nella scala in direzioni PS
			<-ack_membro[id]
			time.Sleep(time.Duration(rand.Intn(5)) * time.Second)
			req_membro[O][PS]<-id
			<-ack_membro[id]
			start_position=PS
		} else {
			req_membro[I][PI]<-id //entra nella scala in direzioni PI
			<-ack_membro[id]
			time.Sleep(time.Duration(rand.Intn(5)) * time.Second)
			req_membro[O][PI]<-id
			<-ack_membro[id]
			start_position=PI
		}

		select {
		default:
			
		case <-termina_membri: //terminazione server
			fmt.Printf("membro %d | termina\n",id)
			done <- true
			return
		}
	}
}

func init_chan() {
	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			req_viaggiatore[i][j]= make(chan int,MAXBUFF) //se prio -> buf
		}
	}

	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			req_membro[i][j]= make(chan int,MAXBUFF) //se prio -> buf
		}
	}

	for i := 0; i < nviaggiatori; i++ { //canali ack per cliente
			ack_viaggiatore[i]= make(chan int,MAXBUFF) 
	}

	for i := 0; i < nmembri; i++ { //canali ack per membro
			ack_membro[i]= make(chan int,MAXBUFF) 
	}
}

func print_chan_len() {
	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			if i+j==2 {
				fmt.Printf("\n\t\t")
			}
			fmt.Printf("req_viaggiatore[%d][%d]: %d\t",i,j,len(req_viaggiatore[i][j]))
		}
	}

	for i := 0; i < 2; i++ { 
		for j := 0; j < 2; j++ { 
			if i+j==2 {
				fmt.Printf("\n\t\t")
				
			}
			fmt.Printf("req_membro[%d][%d]: %d\t",i,j,len(req_membro[i][j]))
		}
	}
	fmt.Printf("\n")
	
}

func main() {

	fmt.Printf("inserisci nviaggiatori (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nviaggiatori)

	fmt.Printf("inserisci nmembri (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nmembri)

	init_chan()

	go scala()

	for i := 0; i < nmembri; i++ {
		go membro(i)
	}

	for i := 0; i < nviaggiatori; i++ {
		go viaggiatore(i)
	}

	//timer per stop applicazione
	time.Sleep(time.Duration(rand.Intn(30)) * time.Second)
	
	//aspetta membri 
	for i := 0; i < nmembri; i++ {
		termina_membri <- true
		<-done
	}

	//aspetta viaggiatori 
	for i := 0; i < nviaggiatori; i++ {
		termina_viaggiatori <- true
		<-done
	}

	//aspetta scala 
	termina_scala <- true
	<-done

	fmt.Printf("FINE\n")
}


	if (	formulaOK(r) && // la formula Ã¨ rispettata 
			!versamentoON(r)) // non ci sono versamenti in corso
		for(i=N-1; i>=0; i--)
			pthread_cond_broadcast(&r->codaS[i]); // risveglio tutti gli spillatori in ordine di prioritÃ 
	else if ((ing==I1) && (!r->sospI1)&&(r->sospI2))
		pthread_cond_broadcast(&r->coda2);

	if (!spillaturaON(r))
	{	if (r->sospI1)
		pthread_cond_broadcast(&r->coda1);
		if (r->sospI2)
			pthread_cond_broadcast(&r->coda2);	
	}

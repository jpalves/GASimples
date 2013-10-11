//Solução do floating ga (Matlab) 0.8917 -15.1644
#include <inttypes.h>
#include <iostream>

#include "ga.h"
#include "math.h"

#define mxIsEmpty(mx) !mxGetM(mx)&&!mxGetN(mx)

#define mxIsScalar(mx) \
	((2 == mxGetNumberOfDimensions(mx)) \
	&& (1 == mxGetM(mx)) && (1 == mxGetN(mx))\
	&& mxIsNumeric(mx) && !mxIsNaN(*(mxGetPr(mx))))

#define isDecimal(num) (bool)(num - (long long) num)


//ainda não pensei nisto

int  integerType(const mxArray *mx){
	int aux =  mxIsInt8(mx)   +
			2*mxIsInt16(mx)   +
			4*mxIsInt32(mx)   +
			8*mxIsInt64(mx)   +
			16*mxIsUint8(mx)  +
			32*mxIsUint16(mx) +
			64*mxIsUint32(mx) +
			128*mxIsUint64(mx); 
	
	return aux;
}

//modificar tornar genérica considerar todas as hipoteses 
inline double positivo(const mxArray *mx,bool inteiro){
	double m = mxGetScalar(mx);
	
	if(inteiro){
		if(!integerType(mx) || integerType(mx)&0xFL){	
			if(isDecimal(m))
				mexErrMsgIdAndTxt("MATLAB:ga3:dims",
                              "números inteiros por favor");
			if(m < 0)
				mexErrMsgIdAndTxt("MATLAB:Ga3:dims",
                                  "números poistivos");				
	}
	} else {
		if(m < 0)
			mexErrMsgIdAndTxt("MATLAB:Ga3:dims",
                                  "números poistivos");	
	}	
	
	return m;
}
//fazer uma função genérica para inteiros e reais fazer um template
template <class T>
inline void verificaIntervalo(int nrhs, const mxArray *prhs[],
                          T &m, unsigned long long i,bool inteiro){
	
	//antes tenho de verifacar se são escalares e double
	if(!mxIsScalar(prhs[i]))
		mexErrMsgIdAndTxt( "MATLAB:ga3:dims",
                           "tem de ser um um escalar");
	//modificar isto
	m = (T) positivo(prhs[i],inteiro);
}


double ffitness(Array<bool> genoma,int nrhs,mxArray **prhs){
	mxArray *lhs,*rhs[2];
	//em teste
	rhs[0] = const_cast<mxArray *>(prhs[0]);
	rhs[1] = mxCreateLogicalMatrix(1,genoma.getDim());
	mxLogical *ptrOut = mxGetLogicals(rhs[1]);
	for(int i=0;i < genoma.getDim();i++)
		ptrOut[i] = (mxLogical) genoma[i];
	
	mexCallMATLAB(1,&lhs,2,rhs,"feval");
	double x = *mxGetPr(lhs);
	mxDestroyArray(rhs[1]);
	mxDestroyArray(lhs);
	return x;
}

//definir parametros de entrada e saida
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
	Cromossoma aux;
	static Random a;
	int i,nMutacoes,nCrossovers,nTorneios,nGladio,tamanhoPop,tamanhoStr,bprint;
	unsigned long long geracoes;
	double pMutacoes,pmMuta,sdMuta,pCrossovers,pmCrossover,sdCrossover,pCrossMutaConsang;
	
	if(!nrhs)
		mexErrMsgTxt("não tem argumentos de entrada");
		
	if( !mxIsClass(prhs[0] , "function_handle")) {
		mexErrMsgTxt("First input argument is not a function handle.");
	}
	//número de gerações
	if(nrhs < 2){
		geracoes = 500;
	} else {
		//verificar se é empty acho que está no escalar 
		verificaIntervalo<unsigned long long>(nrhs,prhs,geracoes,1,1);
		//mexPrintf("%u\n",geracoes);
	}
	//tamanho da polpulação
	if(nrhs < 3){
		tamanhoPop = 50; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,tamanhoPop,2,1);
		//mexPrintf("%u\n",tamanhoPop);
	}
	//tamanho da string cromossoma 
	if(nrhs < 4){
		tamanhoStr = 32; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,tamanhoStr,3,1);
		//mexPrintf("%u\n",tamanhoStr);
	}
	//número de troneios
	if(nrhs < 5){
		nTorneios = 5; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,nTorneios,4,1);
		//mexPrintf("%u\n",nTorneios);
	}
	//número de gladiadores
	if(nrhs < 6){
		nGladio = 15; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,nGladio,5,1);
		//mexPrintf("%u\n",nGladio);
	}
	//número máximo de mutações na população
	if(nrhs < 7){
		nMutacoes = 10; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,nMutacoes,6,1);
		//mexPrintf("%u\n",nMutacoes);
	}
	//probablidade de acontecimento de mutaçãoe
	if(nrhs < 8){
		pMutacoes = 0.2; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,pMutacoes,7,0);
		//mexPrintf("%f\n",pMutacoes);
	}
	//probabildade media de mutação de um gene num cromossoma 
	if(nrhs < 9){
		pmMuta = 0.09; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,pmMuta,8,0);
		//mexPrintf("%f\n",pmMuta);
	}
	//desvio padrão da probabildade media de mutação de um gene num cromossoma 
	if(nrhs < 10){
		sdMuta = 0.05; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,sdMuta,9,0);
		//mexPrintf("%f\n",sdMuta);
	}
	
	//número máximo de Cruzamentos na população
	if(nrhs < 11){
		nCrossovers = 10; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,nCrossovers,10,1);
		//mexPrintf("%u\n",nCrossovers);
	}
	//probablidade de acontecimento de Cruzamentos
	if(nrhs < 12){
		pCrossovers = 0.9; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,pCrossovers,11,0);
		//mexPrintf("%f\n",pCrossovers);
	}
	//probabildade media de mutação de um gene num cromossoma 
	if(nrhs < 13){
		pmCrossover = 0.5; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,pmCrossover,12,0);
		//mexPrintf("%f\n",pmCrossover);
	}
	//desvio padrão da probabildade media de Cruzamento de um gene num cromossoma 
	if(nrhs < 14){
		sdCrossover = 0.05; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,sdCrossover,13,0);
		//mexPrintf("%f\n",sdCrossover);
	}
	
	if(nrhs < 15){
		pCrossMutaConsang = 0.001; 
	} else {
		verificaIntervalo<double>(nrhs,prhs,pCrossMutaConsang,14,0);
		//mexPrintf("%f\n",pCrossMutaConsang);
	}
	
	if(nrhs < 16){
		bprint = 1; 
	} else {
		verificaIntervalo<int>(nrhs,prhs,bprint,15,1);
	}
	Populacao pop(tamanhoPop,tamanhoStr,nrhs,prhs);
	plhs[0] = mxCreateLogicalMatrix(geracoes,tamanhoStr);
	mxLogical *ptrOut = mxGetLogicals(plhs[0]);
	
	//falta implementar -> terminar com critério de convergência
	for(int j=0;j<geracoes;j++){
 		aux=pop.menorFitness();
		for(i=0;i<nCrossovers;i++) if(a.flip(pCrossovers)) pop.crossover(pmCrossover,sdCrossover,pCrossMutaConsang); //as prob vão ser arg de entrada
		for(i=0;i<nMutacoes;i++)   if(a.flip(pMutacoes)) pop.mutacao(pmMuta,sdMuta);
		pop.torneio(nTorneios,nGladio);
		for(int i=0;i < aux.getDim();i++)
			ptrOut[geracoes*i+j] = (mxLogical) aux[i];
		pop.maiorFitness()=aux;
		if(bprint) aux.print();
	}	
}
#include <iostream>
#include "rand.h"
#include "array.h"
#include "mex.h"
//#include <stdlib.h>
//Algoritmo Genético exprimental

//descodificação para float
using namespace std;

double 	ffitness(Array <bool> ,int ,mxArray **);

class Cromossoma:public Array<bool>{
		double fitness;
		
	public:
		//construtores
		Cromossoma(int tamanho):Array<bool>(tamanho){fitness=0;}
		Cromossoma():Array <bool>(){fitness=0;}
		Cromossoma(const Cromossoma &);
      //métodos
		void 			calculaFitness(int nrhs,mxArray **prhs){fitness=ffitness((Array <bool> &) *this,nrhs,prhs);}
		void			setDim(int tamanho);
		double			&getFitness()    {return fitness;}
		void			muta(float prob,Random &R);
		void			print();
		void			flip(int bit);
      //opreadores
      Cromossoma    	operator =(Cromossoma in);

};

Cromossoma::Cromossoma(const Cromossoma &in){
	
	remove(); //não estou certo
	aloca(in.tamanho);
	for(int i=0;i<getDim();i++) array[i] = in.array[i];
	fitness=in.fitness;
}

void Cromossoma::setDim(int tamanho){

	remove();
	aloca(tamanho);
}

void Cromossoma::muta(float prob,Random &R){

	for(int i=0;i<getDim();i++)
		if(R.flip(prob)) this->operator [](i) = !this->operator [](i);
}

Cromossoma Cromossoma::operator =(Cromossoma in){
	int i;

	remove();
	aloca(in.getDim());
	for(i=0;i<getDim();i++) this->operator [](i) = in[i];
	fitness=in.fitness;
	return *this;
}

void Cromossoma::print(){
	for(int i = 0;i < getDim();i++) mexPrintf("%d",(int)this->operator [](i));
	mexPrintf("  ");
	mexPrintf("%f\n",fitness);
}

void Cromossoma::flip(int bit){
  this->operator [](bit) = !this->operator [](bit);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class Populacao{
	Array<Cromossoma> array;
	int n_bits,nrhs;//,n_vars;
	Random a;
	mxArray **prhs;
	
	//métodos privados
	void  		gera(int indice);

	public:
	//Construtores
			Populacao(){}
			Populacao(int tamanho,int n_bits,int nrhs,const mxArray **prhs);
			Populacao(Populacao &);//Cópia do objecto
	//Métodos
	inline int	getDim() {return array.getDim();}
	inline int 	getBits(){return n_bits;}
	Cromossoma  &maiorFitness();
	Cromossoma 	&menorFitness();
	void		setDim(int tamanho,int n_bits,int nrhs,const mxArray *prhs[]); //muda
	void 		torneio(int n_torneios,int n_gladiadores);
	float 		calculaSemelhanca(int i,int j);//incesto dá merda
	void		crossover(double,double,double,double);
	void 		mutacao(double media,double sd){
		int i=a.randArray(array.getDim());
		array[i].muta(a.randn(media,sd),a);
		array[i].calculaFitness(nrhs,prhs);
	}
	void 		mutacaoInversa();
	//operadores
	inline Cromossoma	&operator [](int indice){return array[indice];}
	Populacao		     operator  =(Populacao in);
};

void Populacao::gera(int indice){

	for(int i=0;i<n_bits;i++) array[indice][i]=a.flip(a.rand());
}

Cromossoma &Populacao::maiorFitness(){
	double fit=array[0].getFitness();
	int i;

	for(i=1;i<array.getDim();i++)
		if(array[i].getFitness()>fit) fit=array[i].getFitness();

	for(i=0;i<array.getDim();i++) if(fit==array[i].getFitness()) break;
	return array[i];
}

Cromossoma &Populacao::menorFitness(){
	double fit=array[0].getFitness();
	int i;

	for(i=1;i<array.getDim();i++)
		if(array[i].getFitness()<fit) fit=array[i].getFitness();

	for(i=0;i<array.getDim();i++) if(fit==array[i].getFitness()) break;
	return array[i];
}

//isto é para mudar
void  Populacao::setDim(int tamanho,int n_bits,int nrhs,const mxArray *prhs[]){

	array.setDim(tamanho);
	this->n_bits = n_bits;
	this->nrhs   = nrhs;
	this->prhs   = (mxArray **)prhs;
	for(int i=0;i<tamanho;i++) array[i].setDim(n_bits);

}

//isto é para mudar
void Populacao::torneio(int n_torneios,int n_gladiadores){
	int i,j;//,n_gladiadores=(array.getDim())/2; //mudar -> variável
	Array<Populacao> temp(n_gladiadores);

	if(n_torneios > array.getDim()) n_torneios=array.getDim();
	Array<int> indice(n_torneios);
	
	//escalonamento para o torneio
	for(i=0;i<indice.getDim();i++) indice[i]=a.randArray(array.getDim());

	temp[0].setDim(n_torneios,n_bits,nrhs,(const mxArray **)prhs); //isto muda
	for(i=0;i<temp[0].getDim();i++) temp[0][i]=array[indice[i]];

	for(i=1;i<temp.getDim();i++){
		temp[i].setDim(n_torneios,n_bits,nrhs,(const mxArray**)prhs);
		for(j=0;j<temp[i].getDim();j++) temp[i][j]=array[a.randArray(array.getDim())];
	}
	//torneio
	for(i=1;i<n_gladiadores;i++)
		for(j=0;j<n_torneios;j++)
			if(temp[i][j].getFitness()<temp[0][j].getFitness())
				temp[0][j]=temp[i][j];

	for(i=0;i<n_torneios;i++) array[indice[i]]=temp[0][i];
}

float Populacao::calculaSemelhanca(int i,int j){
	int coincide=0;

	for(int k=0;k<array.getDim();k++)if(array[i][k]==array[j][k]) coincide++;
	return (double)coincide/array.getDim();
}
//tenho de pensar
void Populacao::mutacaoInversa(){
	unsigned long long vitima = a.randArray(array.getDim());
	//array[vitima]
	
	array[vitima].inverte(a.randArray(array[vitima].getDim()),a.randArray(array[vitima].getDim()));
	array[vitima].calculaFitness(nrhs,prhs);
}

//cruzamento uniforme
void Populacao::crossover(double media,double sd,double erro,double pCrossMutaConsangInv){
	Cromossoma	corte(n_bits);//tem de ser pensado
	Populacao	Filho(2,n_bits,nrhs,(const mxArray **)prhs);
	int 		pai=a.randArray(array.getDim()),mae=a.randArray(array.getDim()),i;

	for(i=0;i<corte.getDim();i++)corte[i]=a.flip(a.randn(media,sd));

	for(i=0;i<corte.getDim();i++)
		if(corte[i]){
			Filho[0][i]=array[pai][i];
			Filho[1][i]=array[mae][i];
		} else{
			Filho[1][i]=array[pai][i];
			Filho[0][i]=array[mae][i];
		}

	for(i=0;i<Filho.getDim();i++){ //vou mudar isto, tenho de pensar
		if(calculaSemelhanca(pai,mae) > pCrossMutaConsangInv) //provavel argumento de entrada
			Filho[i].inverte(a.randArray(Filho[i].getDim()),a.randArray(Filho[i].getDim()));
		else Filho[i].muta(calculaSemelhanca(pai,mae)*erro,a);
		Filho[i].calculaFitness(nrhs,prhs);
		array[a.randArray(array.getDim())] = Filho[i];
	}
}
//-Construtores------------------------------------------
//modificar
Populacao::Populacao(Populacao &in){

	array.setDim(in.array.getDim());
	n_bits = in.n_bits;
	nrhs   = in.nrhs;
	prhs   = in.prhs;
	
	for(int i=0;i<array.getDim();i++) array[i]=in.array[i];
}

//em obras tem ser mudado
Populacao::Populacao(int tamanho,int n_bits,int nrhs,const mxArray *prhs[]){

	array.setDim(tamanho);
	this->n_bits = n_bits;
	this->nrhs   = nrhs;
	this->prhs   = (mxArray **)prhs;
	
	for(int i=0;i<tamanho;i++){
		array[i].setDim(n_bits); //ver o aloca do array
		gera(i);
		array[i].calculaFitness(nrhs,(mxArray **)prhs);
	}
}
//-operadores---------------------------------------------------
Populacao  Populacao::operator =(Populacao in){

	   array.setDim(in.array.getDim());
	   n_bits=in.n_bits;
	   for(int i=0;i<array.getDim();i++) array[i]=in.array[i];
	   return *this;
}

//Class Array
#ifndef _ARRAY_H
#define _ARRAY_H

#include <iostream>
#include <cstdlib>

//using namespace std;
//--------------------------------------------------------------------------------------------------------
//definição da classe Array <Tipo de dados> genérico
//--------------------------------------------------------------------------------------------------------
template <typename Tipo> class Array;

template <typename Tipo> std::ostream &operator<<(std::ostream &,const Array<Tipo> &);

template <typename Tipo> class Array{
	protected:
		Tipo *array;
		int tamanho;
	protected:
		void aloca(int);
		void remove();
	public:
		//Construtores
		//nulo
		Array(){array = NULL;tamanho=0;}
		//normal
		Array(int);

		Array(Tipo [],int);

		//cópia do objecto
		Array(const Array<Tipo> &);
		//destrutor
	       ~Array(){remove();}

		//métodos
		//para variar a dimensão durante o programa (depois de declarado)

		//modificar
		void		setDim   (int);
		void		meteAZero(int); //não sei se isto é necessário
		void		copiaPara(Array <Tipo> &);
		void		swap(int,int);
		void 		inverte(int,int);
		//devolve a dimensão
		inline  int	getDim   (){return tamanho;}
		inline  Tipo * 	&cArray  (){return array;}

		//operadores reescritos
		//permite acesso aos elementos atraves do operador deslocamento (a[i])
		Tipo 		   &operator [](int i){return array[i];}
		Array<Tipo> 	operator = (Array <Tipo> in);
		friend std::ostream &operator<< <>(std::ostream &,const Array<Tipo> &);

};
//--------------------------------------------------------------------------------------------------------
//Métodos Privados
//--------------------------------------------------------------------------------------------------------
template <typename Tipo> void Array<Tipo>::aloca(int tamanho){

	if(!(array = new Tipo[tamanho])){ //refazer para windows
		std::cout <<"Erro na Memória"<<std::endl;
		exit(1);
	}
	this->tamanho=tamanho;
}

template <typename Tipo> void Array<Tipo>::remove(){
		if(array){
			delete[] array;
			array = NULL; //acho que isto não é necessário
		}
}
//--------------------------------------------------------------------------------------------------------
//Construtores
//--------------------------------------------------------------------------------------------------------

template <typename Tipo> Array<Tipo>::Array(int tamanho){
	aloca(tamanho);
}

template <typename Tipo> Array<Tipo>::Array(Tipo in[],int tamanho){
	aloca(tamanho);

	for(register int i=0;i < tamanho;i++) *(array+i) = *(in+i);
	this->tamanho=tamanho;
}
//cópia do objecto
template <typename Tipo> Array<Tipo>::Array(const Array<Tipo> &in){

       			aloca(in.tamanho);
       			for(register int i = 0;i < tamanho;i++) *(array+i) = *(in.array+i);
}
//--------------------------------------------------------------------------------------------------------
//Operadores
//--------------------------------------------------------------------------------------------------------
template <typename Tipo> Array<Tipo> Array<Tipo>::operator  = (Array <Tipo> in){
			register Tipo *ptr1 = in.array;

			if(this != &in){//não modifica a cópia dele mesmo
				remove();
				aloca(in.tamanho); //fica com o tamanho do outro
				for(register Tipo *ptr = array;(ptr - array) -  tamanho;) *ptr ++ = *ptr1 ++;
			}
			return *this;
}
//--------------------------------------------------------------------------------------------------------
//Outros Métodos
//--------------------------------------------------------------------------------------------------------
template <typename Tipo> void Array<Tipo>::meteAZero(int dim){
			register Tipo *ptr = array;

			for(;(ptr - array) -  dim;) *ptr ++  =  (Tipo)NULL;
}
//--------------------------------------------------------------------------------------------------------
template <typename Tipo> void Array<Tipo>::copiaPara(Array <Tipo> &in){
			register int i=0;

			for(;in.tamanho-i&&tamanho-i;i++) *(in.array+i) = *(array+i);
			for(;i<in.tamanho;) *(in.array + i++) = (Tipo)NULL;
}

template <typename Tipo> void Array<Tipo>::setDim(int tamanho){
	remove();
	aloca(tamanho);
}

template <typename Tipo> void Array<Tipo>::swap(int i,int j){
	Tipo aux = array[i];
	array[i] = array[j];
	array[j] = aux;
}

template <typename Tipo> void Array<Tipo>::inverte(int i,int j){
	if(i > j){
		int aux = i;
		i = j;
		j = aux;
	}

	int pMedio = (i + j)/2,l = 0;
	for(int k = i;k <= pMedio ;k++,l++)
		swap(k,j-l);
}

//---------------------------------------------------------------------------------------------------------
//Funções friend
//---------------------------------------------------------------------------------------------------------
template <typename Tipo> std::ostream &operator <<(std::ostream & out, const Array<Tipo> &in){
	out <<'(';
	for(int i=0;i<in.tamanho-1;i++) out<<in.array[i]<<',';
	out<<in.array[in.tamanho-1]<<')'<<std::endl;

	return out;
}
#endif

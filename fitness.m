function vFitness = fitness(genoma)
    varInt  = sum(genoma.*2.^((length(genoma)-1):-1:0));
    maxInt  = 2.^length(genoma)-1;
    zeroaum = (maxInt - varInt)/maxInt;
    x = zeroaum*2*pi; 
    
    vFitness = x+10*sin(5*x)+7*cos(4*x);
    
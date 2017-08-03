function [A] = matA(tt)
    A = [0 1; -(2+5*tt) -(3+2*tt)];
endfunction

t0=0;
tf=1;
dt=0.01;
t=t0:dt:tf;
x0 = [0;0];
xint = [x0];
ref = [];
ek = [];
A = [0 1; -1 -2];
B = [0;1];
C = [0 1];
u = 1;
Q = 1;
R = 0.1;
F = 0.00744;
//------------------------------------------------------------------------------
//calculating the gains
K = [];
k0 = C'*F*C;
for k=1:length(t)    
    kk = A'*k0 + k0*A - k0*B*inv(R)*B'*k0 + C'*Q*C;
    K = [K kk];
    k0 = kk;
end
//------------------------------------------------------------------------------
for k=1:length(t)
        Aa = matA(t(k));
        xint = [xint Aa*x0 + B*u];
        ref = [ref 12*t(k)^2*(1-t(k))];
end
//------------------------------------------------------------------------------
b = B*inv(R)*B';
c = C'*Q*C;
kkk = riccati(A,b,c,'c');

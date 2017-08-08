//------------------------------------------------------------------------------
// FEDERAL UNIVERSITY OF UBERLANDIA
// Faculty of Electrical Engineering
// Biomedical Engineering Lab
// Uberlandia, Brazil
//------------------------------------------------------------------------------
// Author: Andrei Nakagawa, MSc
// Contact: andrei.ufu@gmail.com
//------------------------------------------------------------------------------
// Description: Simulating force-field adaptation experiments
// Reproducing the paper from Todorov (2005)
//"Stochastic optimal control and estimation methods adapted
//to the noise characteristics of the sensorimotor system"
//------------------------------------------------------------------------------
// Optimal controller
// Gain (L) = inv(R + B'*Sx(k+1)*B + sum(Ci'*Sx(k+1)*Se(k+1)*Ci) * B'*Sx(k+1)*A
// Sx(k) = Q + A'*Sx(k+1)*(A - B*L) + sum(Di'*K'*Se(k+1)*K*Di
// Se(k) = A'*Sx(k+1)*B*L + (A - K*H)' * Se(k+1) * (A - K*H)
// st = trace(Sx(k+1)*ohmPsi + Se(k+1)*(ohmPsi+ohmNepta + K*ohmOmega*K')) + st(k+1)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
clear;
clc;
//------------------------------------------------------------------------------
function [Ad,Bd,Cd] = pointMassModel(m,dt,tau1,tau2)
    Ad = [1 dt 0 0 0; 0 1 dt/m 0 0; 0 0 1-(dt/tau2) dt/tau2 0; 0 0 0 1-(dt/tau1) 0; 0 0 0 0 1];
    Bd = [0;0;0;dt/tau1;0];
    Cd = [1 0 0 0 0; 0 1 0 0 0; 0 0 1 0 0];
endfunction
//------------------------------------------------------------------------------
//Dynamics:
//Ax(k+1) + Bu(k) + sum(B*C*u(k))
//Y(k) = Hx(k) + w(k)
//Where:
//C: Signal-dependent noise scaling
//D: State-dependent noise scaling
//C0: Signal-dependent noise covariance
//D0: State-dependent noise covariance
//E0: 
function [L,K] = todorovOFC(A,B,H,C,C0,D,D0,E0,Q,R,X0,SX0)    
    //Convergence parameters
    //Difference between cost
    minError = 1e-15;
    //Maximum iterations
    maxIter = 500;
    
    //Number of states
    sizeX = size(A,1);
    //Number of control inputs
    sizeU = size(B,2);
    //Number of output variables
    sizeY = size(H,1);
    
    sizeC = size(C,3);
    sizeC0 = size(C0,2);
    sizeD = size(D,3);
    sizeD0 = size(D0,2);
    sizeE0 = size(E0,2);
    N = size(Q,3);
    
    if(size(C,1)==1 & sizeU > 1)
        C = C * ones(sizeU,1);
    end
    
    if(length(D(:)) == 1)
        if D(1) == 0
            D = zeros(sizeY,sizeX);
        else
            D= D*ones(sizeX,1);
        end
    end
    
    if length(C0) == 1 & C0(1) == 0 then
        C0 = zeros(sizeX,1);
    end
    
    if length(D0) == 1 & D0(1) == 0 then
        D0 = zeros(sizeY,1);
    end
    
    if length(E0) == 1 & E0(1) == 0 then
        E0 = zeros(sizeX,1);
    end
    
    L = zeros(sizeU,sizeX,N-1);
    K = zeros(sizeX,sizeY,N-1);
    
    //run algorithm until convergence or for a maximum number of iterations
    for it=1:maxIter
        //Initial condition
        //Initial error covariance
        Ske = SX0;
        //Initial state covariance
        Skx = X0*X0';
        //Combined state and error covariance
        Skxe = zeros(sizeX,sizeX);
        //Iterative algorithm to find L and K
        //Compute the Kalman gain
        for k=1:N-1        
            temp = Ske + Skx + Skxe + Skxe';
            DSiD = zeros(sizeY,sizeY);
            for i=1:sizeD
                DSiD = DSiD + D(:,:,i)*temp*D(:,:,i)';
            end
            //kalman gain
            K(:,:,k) = A*Ske*H' * inv(H*Ske*H' + D0*D0' + DSiD);
            //update covariances
            newE = E0*E0' + C0*C0' + (A - K(:,:,k)*H)*Ske*A';
            LSiL = L(:,:,k)*Skx*L(:,:,k)';
            for i=1:sizeC
                newE = newE + (B*C(:,:,i))*LSiL*(B*C(:,:,i))';
            end
            Skx = E0*E0' + K(:,:,k)*H*Ske*A' + (A-B*L(:,:,k))*Skx*(A-B*L(:,:,k))' + (A-B*L(:,:,k))*Skxe*H'*K(:,:,k)' + K(:,:,k)*H*Skxe'*(A-B*L(:,:,k))';
            Ske = newE;
            Skxe = (A-B*L(:,:,k))*Skxe*(A-K(:,:,k)*H)' - E0*E0';
        end

        //Optimal controller
        //Initialize optimal cost-to-go function
        Sx = Q(:,:,N);
        Se = zeros(sizeX,sizeX);
        Cost(it) = 0;
        
        for k=N-1:-1:1
            //cost
            Cost(it) = Cost(it) + trace(Sx*C0*C0') + trace(Se*(K(:,:,k)*D0*D0'*K(:,:,k)' + E0*E0' + C0*C0'));
            //controller
            temp = R + B'*Sx*B;
            BSxeB = B'*(Sx+Se)*B;
            for i=1:size(C,3)
                temp = temp + C(:,:,i)'*BSxeB*C(:,:,i)
            end            
            //controller gain
            L(:,:,k) = inv(temp) * B'*Sx*A;
            //update sx and se
            newE = A'*Sx*B*L(:,:,k) + (A - K(:,:,k)*H)'*Se*(A - K(:,:,k)*H);
            Sx = Q(:,:,k) + A'*Sx*(A - B*L(:,:,k));
            KSeK = K(:,:,k)'*Se*K(:,:,k);
            for i=1:sizeD
                Sx = Sx + D(:,:,i)'*KSeK*D(:,:,i);
            end
            Se = newE;            
        end
        
        //adjust cost
        Cost(it) = Cost(it) + X0'*Sx*X0 + trace((Se+Sx)*SX0);
        disp(it);
        //Check convergence of cost
        if(it > 1 & abs(Cost(it)-Cost(it)) < minError)
            break;        
        end
    end        
endfunction
//------------------------------------------------------------------------------
//Function for simulating the experiment according to the
//optimal controller (L) and optimal estimator (K)
function [xsim,usim,unoise,xp,yp] = runSimulationOFC(A,B,H,L,K,C,C0,D,D0,E0,x0,sx0,t)
    //Number of states
    sizeX = size(A,1);
    //Number of control inputs
    sizeU = size(B,2);
    //Number of output variables
    sizeY = size(H,1);
    
    sizeC = size(C,3);
    sizeC0 = size(C0,2);
    sizeD = size(D,3);
    sizeD0 = size(D0,2);
    sizeE0 = size(E0,2);
    N = size(Q,3);
    
    if(size(C,1)==1 & sizeU > 1)
        C = C * ones(sizeU,1);
    end
    
    if(length(D(:)) == 1)
        if D(1) == 0
            D = zeros(sizeY,sizeX);
        else
            D= D*ones(sizeX,1);
        end
    end
    
    if length(C0) == 1 & C0(1) == 0 then
        C0 = zeros(sizeX,1);
    end
    
    if length(D0) == 1 & D0(1) == 0 then
        D0 = zeros(sizeY,1);
    end
    
    if length(E0) == 1 & E0(1) == 0 then
        E0 = zeros(sizeX,1);
    end
    N = size(L,3);
    xsim = [x0];
    usim = [];
    unoise = [];
    xp = [x0];
    yp = [0;0;0];
    x=x0;
    xhat = x0;    
    for k=1:length(t)-1
        //control
        u = -L(:,:,k)*xhat;
        //noisy control
        un = u;
        for i=1:length(u)
            un(i) = un(i) + (C*u(i)*rand(1,'normal'));
        end
        //observation
        y = H*x + D0*rand(sizeY,1,'normal');
        x = A*x + B*un;
        xhat = A*xhat + B*u + K(:,:,k)*(y - H*xhat);
        xsim = [xsim x];
        usim = [usim u];
        unoise = [unoise un];
        xp = [xp xhat];
        yp = [yp y];
    end
endfunction
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Time step
dt = 0.001;
//Simulation parameters
t0 = 0; //initial time
tf = dt*400; //final time
t = t0:dt:tf; //Time vector
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Plant model
m=1;
t1 = 0.04; t2 = 0.04;
[A,B,H] = pointMassModel(m,dt,t1,t2);
//------------------------------------------------------------------------------
//Parameters
//Weight matrices
//state
Q = zeros(5,5,length(t));
wv = 0.2; //velocity
wf = 0.02; //force
costPosition = [1 0 0 0 -1];
costVelocity = [0 wv 0 0 0];
costForce = [0 0 wf 0 0];
finalCost = [costPosition;costVelocity;costForce];
Q(:,:,$) = finalCost'*finalCost;
//control
R = 0.00001 / (length(t)-1);
//noise
//output noise
noisePos = 0.02;
noiseVel = 0.2;
noiseForce = 1;
covOmega = 0.5 * diag([noisePos,noiseVel,noiseForce]);
//control-dependet noise
roC = 0.5;
//state-dependent noise - absent from this simulation
//why not used?
//inital state variance
varx0 = 0;
covx0 = zeros(5,5);
//target
target = 0.2;
//initial state
x0 = [0;0;0;0;target];
//------------------------------------------------------------------------------
//Finding the optimal controller and optimal estimator
[LL,KK] = todorovOFC(A,B,H,roC,0,0,covOmega,0,Q,R,x0,covx0);
//------------------------------------------------------------------------------
//Average trajectory - no noise added
xint = [x0];
x = x0;
for k=1:length(t)-1
    u = -LL(:,:,k)*x;
    x = A*x + B*u;
    xint = [xint x];
end
for i=1:1
    [xsim,usim,unoise,xp,yp] = runSimulationOFC(A,B,H,LL,KK,roC,0,0,covOmega,0,x0,covx0,t);
    plot(t,xsim(1,:),'r');    
    plot(t,xp(1,:),'b');
    plot(t,yp(1,:),'g');
end
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
////Finding optimal L and K
//K = [];
//LL = zeros(1,5);
//for k=1:length(t)
//    //optimal estimation
//    //covariances
//    sie = covx0;
//    six = x0*x0';
//    sixe = zeros(size(x0,1),size(x0,1));
//    //kalman filter
//    //kalman gain
//    kk = A*sie*H' * inv(H*sie*H' + covOmega);
//    K = [K kk];
//    newE = (A - kk*H)*sie*A';
//    for n=1:length(roC)
//        newE = newE + B*roC(n)*LL*six*LL'*(B*roC(n))';
//    end
//    six = kk*H*sie*A' + (A-B*LL)*six*(A-B*LL)' + (A-B*LL)*sixe*H'*kk' + kk*H*sixe*(A-B*LL)';
//    sie = newE;
//    sixe = (A-B*LL)*sixe*(A-kk*H)';    
//end
////optimal cost-to-go function
//Sx = Q(:,:,$);
//Se = zeros(5,5);
//
//for k=length(t)-1:-1:1
//    //optimal control
//    Ll = R + B'*Sx*B;
//    for n=1:length(roC)
//        Ll = Ll + (B*roC)'*(B'*(Sx+Se)*B)*(B*roC);
//    end
//    LL = inv(Ll) + B'*Sx*A;
//    newX = Q(:,:,k) + A'*Sx*(A-B*Ll);
//    newE = A'*Sx*B*Ll + (A-K(k)*H)'*Se*(A-K(k)*H);
//    Se = newE;
//    Sx = newX;
//end
//    
////------------------------------------------------------------------------------
////Testing signal-dependent noise
////uint = [];
////uint2 = [];
////u0 = 1;
////u1 = 10;
////for k=1:length(t)
////    ux = B*u0 + (B*u0*roC*rand(1,'normal'));
////    uint = [uint ux];
////    ux = B*u1 + (B*u1*roC*rand(1,'normal'));
////    uint2 = [uint2 ux];
////end
////------------------------------------------------------------------------------
//

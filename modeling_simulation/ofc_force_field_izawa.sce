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
// Reproducing results from Izawa et al. (2008)
//------------------------------------------------------------------------------
clear;
clc;
//------------------------------------------------------------------------------
//Kalman filter
function [xplus,pplus] = kalman(F,G,H,Q,R,xk,pk,yk,uk)
    //time update (prediction)
    xminus = F*xk + G*uk;
    pminus = F*pk*F' + G*Q*G';    
    //measurement update (correction)    
    measureError = yk - (H*xminus);
    kalmanGain = pminus*H' * inv(H*pminus*H' + R);
    //state estimate
    xplus = xminus + kalmanGain * measureError;
    //covariance
    pplus = pminus - kalmanGain*H*pminus;
endfunction
//------------------------------------------------------------------------------
//States
//Position and velocity in X
//Position and velocity in Y
//Inputs
//Force in X and Y
function [Ac,Bc,Cc] = pointMassModel(m)
    Ac = [0 1 0 0; 0 0 0 0; 0 0 0 1; 0 0 0 0];
    Bc = [0 0; 1/m 0; 0 0; 0 1/m];
    Cc = eye(size(Ac,1),size(Ac,2))
endfunction
//------------------------------------------------------------------------------
function [Ak,Bk,Ck] = discPointMassModel(m,dt)
    Ak = [1 dt 0 0; 0 1 0 0; 0 0 1 dt; 0 0 0 1];
    Bk = [0 0; dt/m 0;0 0;0 dt/m];
    Ck = eye(size(Ak,1),size(Ak,2));
endfunction
//------------------------------------------------------------------------------
//Model of the plant during force field experiments
//Force applied by the robot is velocity-dependent
function [Ak,Bk,Ck] = discPointMassFFModel(m,dt,Dv,tau)
    a1 = [1 dt 0 0 0 0 0 0];
    a2 = [0 1+(dt*(Dv(1,1)/m)) 0 dt*(Dv(1,2)/m) dt/m 0 0 0];
    a3 = [0 0 1 dt 0 0 0 0];
    a4 = [0 (dt*(Dv(2,1)/m)) 0 dt*(Dv(2,2)/m) 0 dt/m 0 0];
    a5 = [0 0 0 0 1 - (dt/tau) 0 0 0];
    a6 = [0 0 0 0 0 1 - (dt/tau) 0 0];
    a7 = [0 0 0 0 0 0 1 0];
    a8 = [0 0 0 0 0 0 0 1];
    Ak = [a1;a2;a3;a4;a5;a6;a7;a8];
    Bk = [0 0; 0 0; 0 0; 0 0; dt/tau 0; 0 dt/tau; 0 0; 0 0];
    Ck = eye(size(Ak,1),size(Ak,2));
endfunction
//------------------------------------------------------------------------------
//Simulation parameters
//Time
//initial time
t0 = 0;
//final time
tf = 5;
//time step
dt = 0.01;
//time vector
t = t0:dt:tf;
//------------------------------------------------------------------------------
//Force-field
Dv = [0 13; -13 0];
Dnull = [0 0; 0 0];
//Mass
m=4;
//time-constant
tau = 0.120;
[Ad,Bd,Cd] = discPointMassFFModel(m,dt,Dnull,tau);
[Ak,Bk,Ck] = discPointMassFFModel(m,dt,Dv,tau);
//------------------------------------------------------------------------------
//Optimal feedback control
//------------------------------------------------------------------------------
//Differential Riccati Equation - Discrete-time
//Calculating the solution to riccati for each instant in time
//and then finding the time-varying gain for each time step
//------------------------------------------------------------------------------
Sdisc = [];
Kdisc = [];
//Weight matrices
Qd = zeros(8,8);
Qd(1,1) = 5; Qd(1,7) = -5; Qd(3,3) = 5; Qd(3,8) = -5;
Qd(2,2) = 0.01; Qd(4,4) = 0.01;
Qd(5,5) = 0; Qd(6,6) = 0;
Qd(7,1) = -5; Qd(7,7) = 5;
Qd(8,3) = -5; Qd(8,8) = 5;
Rd=diag([1e-5,1e-5]);
S0 = diag([500,0,120,0,0,0,10,10]); //Estimate for the Riccati matrix 
for k=1:length(t)
    //Calculating the time-varying gain
    K = inv(Bd'*S0*Bd + Rd)*(Bd'*S0*Ad);
    //New riccati solution
    S0 = Ad'*S0*Ad - Ad'*S0*Bd*((Rd + Bd'*S0*Bd)^-1)*Bd'*S0*Ad + Qd;
    //Stores the riccati solution
    Sdisc = [Sdisc S0];    
    //Stores the gain
    Kdisc = [Kdisc K];
end
//------------------------------------------------------------------------------
//OFC simulation
//LQG parameters
//Initial condition
tx = 0; ty=0.5;
x0 = [0;0;0;0;0;0;tx;ty];
xd = [0;tx;ty;0;0;0;tx;ty];
x = x0;
xopt = [];
uopt = [];
frint = [];
cont=1;
//kalman parameters
q = zeros(2,2);
r = eye(8,8);
x00=x0;
p0 = eye(8,8);
xp = [];
yk = [x0];
yn = [x0];
xpp=x0;
//Simulation
for k=1:length(t)  
    //input - motor commands
    u = -Kdisc(:,cont:cont+7)*(x-xd);
    //new states
    x = Ad*x + Bd*u;
    
    //force produced by the robot - perturbation
    frx = dt*((Dv(1,1)/m)*x(2)) + dt*((Dv(1,2)/m)*x(4));
    fry = dt*((Dv(2,1)/m)*x(2)) + dt*((Dv(2,2)/m)*x(4));
    frint = [frint [frx;fry]];
        
    //saving the optimal trajectory
    xopt = [xopt x];
    uopt = [uopt u];
    cont = cont+8;
end
figure(); plot(xopt(1,:),xopt(3,:));
ax=gca();
ax.data_bounds=[-0.2 -0.1; 0.2 ty+0.1];
//------------------------------------------------------------------------------
//Iterative learning control
numTrials = 50;
alpha = 0.9;
erro = zeros(uopt);
up = uopt;
for n=1:10
    xint = [];
    uint = up;
    up = [];
    cont=1;
    x = x0;
    //kalman parameters
    q = zeros(2,2);
    r = eye(8,8);
    x00=x0;
    p0 = eye(8,8);
    xp = [];
    yk = [x0];
    yn = [x0];
    xpp=x0;
    frint = [];
    for k=1:length(t)
        //input - motor commands
        u = 0.9*uint(:,k) + 0.1 * erro(:,k);
        up = [up u];
        //new states
        x = Ak*x + Bk*u;
        
        //optimal state estimation - kalman filter
        //output
        ykk = Ck * x;
                
        //adding noise
        for i=1:length(ykk)
            ynn(i) = ykk(i) + 0.01 * rand(1,'normal');
        end
    
        //kalman filter
        [xpp,pp] = kalman(Ak,Bk,Ck,q,r,x00,p0,ynn,u);
        
        //force produced by the robot - perturbation
        frx = dt*((Dv(1,1)/m)*x(2)) + dt*((Dv(1,2)/m)*x(4));
        fry = dt*((Dv(2,1)/m)*x(2)) + dt*((Dv(2,2)/m)*x(4));
        frint = [frint [frx;fry]];

        erro(1,k) = u(1) + frint(1,k);
        erro(2,k) = u(2) + frint(2,k);
        
        //updating variables - state estimation
        yk = [yk ykk];
        yn = [yn ynn];
        xp = [xp xpp];
        x00 = xpp;
        p0 = pp;    
        
        //updating variables - control
        xint = [xint x];
        uint = [uint u];
        cont = cont+8;
    end
    
    figure();
    plot(xint(1,:),xint(3,:));
    plot(xd(1),xd(3),'r.');
    figure();
    plot(frint(1,:)); plot(up(1,:),'r'); plot(erro(1,:),'g');
//    ax=gca();
//    ax.data_bounds=[-0.2 -0.1; 0.2 ty+0.1];
end
//------------------------------------------------------------------------------
//close all figures
//xdel(winsid())
//ax=gca();
//ax.data_bounds=[-0.5 -0.5; 1 6];
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

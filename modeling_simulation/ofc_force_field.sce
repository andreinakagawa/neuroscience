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
// Reproducing results from Ostry et al. (2010)
//------------------------------------------------------------------------------
clear;
clc;
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
function [Ak,Bk,Ck] = discPointMassFFModel(m,dt,Dv)
    Ak = [1 dt 0 0; 0 1+(dt*(Dv(1,1)/m)) 0 dt*(Dv(1,2)/m); 0 0 1 dt; 0 dt*(Dv(2,1)/m) 0 1+(dt*(Dv(2,2)/m))];
    Bk = [0 0; dt/m 0;0 0;0 dt/m];
    Ck = eye(size(Ak,1),size(Ak,2));
endfunction
//------------------------------------------------------------------------------
//Simulation parameters
//Time
//initial time
t0 = 0;
//final time
tf = 10;
//time step
dt = 0.01;
//time vector
t = t0:dt:tf;
//------------------------------------------------------------------------------
//Initial condition
x0 = [0;0;0;0];
x = x0;
//Force-field
Dv = [0 3; -3 0];
//Mass
m=2;
[Ad,Bd,Cd] = discPointMassFFModel(m,dt,Dv);
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
Qd=diag([0.1,0.1,0.1,0.1]);
Rd=diag([0.01,0.01]);
S0 = diag([500,0,120,0]); //Estimate for the Riccati matrix 
xd = [0;0;5;0];
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
xint = [];
uint = [];
frint = [];
x=[0;0;0;0];
cont=1;
for k=1:length(t)
    frx = dt*((Dv(1,1)/m)*x(2)) + dt*((Dv(1,2)/m)*x(4));
    fry = dt*((Dv(2,1)/m)*x(2)) + dt*((Dv(2,2)/m)*x(4));
    u = -Kdisc(:,cont:cont+3)*(x-xd);
    x = Ad*x + Bd*u;
    xint = [xint x]; 
    uint = [uint u];
    frint = [frint [frx;fry]];
    cont = cont+4;
end
//------------------------------------------------------------------------------
figure();
plot(xint(1,:),xint(3,:));
plot(xd(1),xd(3),'r.');
ax=gca();
ax.data_bounds=[-0.5 -0.5; 1 6];
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

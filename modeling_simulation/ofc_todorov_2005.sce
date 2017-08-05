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
//Time step
dt = 0.01;
//Simulation parameters
t0 = 0; //initial time
tf = 1; //final time
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
R = 0.00001;
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
target = 0.1;
//initial state
x0 = [0;0;0;0;target];
//------------------------------------------------------------------------------
//Finding optimal L and K
K = [];
for k=1:length(t)
    //optimal estimation
    //covariances
    sie = covx0;
    six = x0*x0';
    sixe = zeros(size(x0,1),size(x0,1));
    //kalman filter
    //kalman gain
    kk = A*sie*H' * inv(H*sie*H' + covOmega);
    K = [K kk];
    sie = (A - kk*H)*sie*A' + roC*L*six
    //optimal control
end
//------------------------------------------------------------------------------
//Testing signal-dependent noise
//uint = [];
//uint2 = [];
//u0 = 1;
//u1 = 10;
//for k=1:length(t)
//    ux = B*u0 + (B*u0*roC*rand(1,'normal'));
//    uint = [uint ux];
//    ux = B*u1 + (B*u1*roC*rand(1,'normal'));
//    uint2 = [uint2 ux];
//end

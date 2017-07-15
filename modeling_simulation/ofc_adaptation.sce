//------------------------------------------------------------------------------
// FEDERAL UNIVERSITY OF UBERLANDIA
// Faculty of Electrical Engineering
// Biomedical Engineering Lab
// Uberlandia, Brazil
//------------------------------------------------------------------------------
// Author: Andrei Nakagawa, MSc
// Contact: andrei.ufu@gmail.com
//------------------------------------------------------------------------------
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
[A,B,C] = pointMassModel(1);
//------------------------------------------------------------------------------
//Simulation parameters
t0=0;
tf=3;
dt = 0.01;
t = t0:dt:tf;
//Continuous-time system
contSys = syslin('c',A,B,C);
//Discrete-time system
discSys = dscr(contSys,dt);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Weight matrices
Qd=diag([0.1,0.1,0.1,0.1]);
Rd=diag([0.01,0.01]);
//Discrete riccati
Ad = discSys(2); //A
Bd = discSys(3); //B
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Differential Riccati Equation - Discrete-time
//Calculating the solution to riccati for each instant in time
//and then finding the time-varying gain for each time step
//------------------------------------------------------------------------------
Sdisc = [];
Kdisc = [];
S0 = diag([500,0,120,0]); //Estimate for the Riccati matrix 
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
cont = 1;
//Desired setpoints or reference trajectory
xd = [0;0;5;0];
xint = []; //stores all the states during integration
uint = []; //stores all the inputs during integration
costQ = [0]; //cost of states
costR = [0]; //cost of control
x0 = [0;0;0;0]; //temporary variable for storing states
u0 = [0;0];
x = x0;
xint = [xint x0];
uint = [uint u0];
upp = zeros(2,length(t));
//------------------------------------------------------------------------------
for k=1:length(t)-1
    //Calculating the input
    u = -Kdisc(:,cont:cont+3) * (x-xd);
    //Calculating the new states
    x = Ad*x + Bd*u;
    //Storing the new states
    xint = [xint x];
    //Storing the new inputs
    uint = [uint u];
    //Stores the cost in this step
    costQ = [costQ (x-xd)'*Qd*(x-xd)];
    //Stores the cost in this step
    costR = [costR u'*Rd*u];
    //Increments the counter to loop through the gain matrix
    cont = cont + 4;
end
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
figure();
plot(xint(1,:),xint(3,:),'k');
plot(xint(1,$),xint(3,$),'k.');
plot(x0(1),x0(3),'b.');
plot(xd(1),xd(3),'r.');
ax=gca();
ax.data_bounds=[-5 -5; 5 5];
figure();
plot(t,xint(2,:),'r');
plot(t,xint(4,:),'g');
plot(t,uint(1,:),'k');
plot(t,uint(2,:),'k');
figure();
plot(t,costQ,'r');
plot(t,costR,'b');
//------------------------------------------------------------------------------

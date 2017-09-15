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
// Description: Combining a sensorimotor transformation
// function with optimal feedback control to explain online
// corrections to visual perturbations. 
//------------------------------------------------------------------------------
// Algorithm: Before running the simulation, find the optimal
// time-varying feedback gains according to the task goal.
// Using a delayed visual feedback, whenever target location
// in motor space changes, the gains should be recomputed
// given the amount of time left to complete the reach and
// applied to the simulation. Corrections are only necessary
// if cursor is not inside the target (to avoid error oscillations).
// Delayed feedback and reaction times will be combined
// (see Li and Todorov, 2007) with a constant value (200 ms).
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
//Optimal time-varying feedback gains
function Kopt = computeGain(Ad,Bd,Qd,Rd,S0,time)
    Kopt = [];
    for k=1:length(time)
        //Calculating the time-varying gain
        K = inv(Bd'*S0*Bd + Rd)*(Bd'*S0*Ad);
        //New riccati solution
        S0 = Ad'*S0*Ad - Ad'*S0*Bd*((Rd + Bd'*S0*Bd)^-1)*Bd'*S0*Ad + Qd;
        //Stores the gain
        Kopt = [Kopt K];
    end
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
//Function that specifies the task goal in motor space according to visual space
function mtarget=smTransf(vcursor, vtarget, mhand)
    //movement vector in visual space
    vvector = vtarget - vcursor;
    //magnitude
    magnitude = norm(vvector);
    //direction
    if(vvector(1) == 0)
        direction = (90*%pi)/180;
    else
        direction = abs(atan(vvector(2)/vvector(1)));
    end
    //estimating target location in motor space
    mtarget = zeros(1,2); 
    //x-axis    
    mtarget(1) = mhand(1) + (sign(vvector(1)) * magnitude * cos(direction));
    //y-axis
    mtarget(2) = mhand(2) + (sign(vvector(2)) * magnitude * sin(direction));
endfunction
//------------------------------------------------------------------------------
[A,B,C] = pointMassModel(1);
//------------------------------------------------------------------------------
//Simulation parameters
t0=0;
tf=6;
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
Rd=diag([0.001,0.001]);
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
S0 = diag([50,0,50,0]); //Estimate for the Riccati matrix 
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
vtarget = [xd(1),xd(3)];
xint = []; //stores all the states during integration
uint = []; //stores all the inputs during integration
costQ = [0]; //cost of states
costR = [0]; //cost of control
x0 = [0;0;0;0]; //temporary variable for storing states
u0 = [0;0];
x = x0;
xint = [xint x0];
uint = [uint u0];
//Perturbation
//Rotation matrix
perturbation = 30; //degrees
Ck = [cos((perturbation*%pi)/180) 0 -sin((perturbation*%pi)/180) 0; sin((perturbation*%pi)/180) 0 cos((perturbation*%pi)/180) 0];
yint = [x0];
yaux = x0;
//Delayed feedback and reaction time
delay = 0.200; //Every 200 ms, target in motor space will be updated
t0 = t(1); //first time-step
//Target jump
targetJump = 0;
//------------------------------------------------------------------------------
for k=1:length(t)-1
    
    //Calculating the input (force)
    u = -Kdisc(:,cont:cont+3) * (x-xd);
    //Calculating the new states
    x = Ad*x + Bd*u;
    
    //perturbation
    y = Ck*x;
    y = [y(1);0;y(2);0];
    aux = [yint y];
    yvx = diff(yint(1,:));
    if yvx == [] then
        yvx = 0;
    end
    
    yvy = diff(yint(3,:));
    if yvy == [] then
        yvy = 0;
    end
    yaux(1) = y(1);
    yaux(2) = yvx($);
    yaux(3) = y(3);
    yaux(4) = yvy($);
    yint = [yint yaux];
    
    //Checks if visual feedback should be updated
    if(t(k) - t0 >= delay)
        t0 = t(k);
        vcursor = [yint(1,k),yint(3,k)];
        //vtarget = [xd(1), xd(3)];
        mhand = [xint(1,k),xint(3,k)];
        motorTarget = smTransf(vcursor,vtarget,mhand);
        if(motorTarget ~= [xd(1),xd(3)])
            xd(1) = motorTarget(1);
            xd(3) = motorTarget(2);     
            newTime = t(k:$);
            Kdisc = computeGain(Ad,Bd,Qd,Rd,S0,newTime);
            cont = 1;
            disp('updated');
        end
    end
    
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
plot(yint(1,:),yint(3,:),'b');
plot(xint(1,$),xint(3,$),'k.');
plot(vtarget(1),vtarget(2),'r.');
plot(x0(1),x0(3),'b.');
xlabel('x-axis');
ylabel('y-axis');
legend('Hand movement', 'Cursor motion');
ax=gca();
ax.data_bounds=[-3 -1; 3 6];
figure();
plot(t,xint(2,:),'r');
plot(t,xint(4,:),'g');
plot(t,uint(1,:),'k');
plot(t,uint(2,:),'k');
figure();
plot(t,costQ,'r');
plot(t,costR,'b');
title("Cost");
//------------------------------------------------------------------------------

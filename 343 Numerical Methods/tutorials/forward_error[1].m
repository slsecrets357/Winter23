
n  = 5;  % size of the matrix 
%% this part of the code creates a ill-conditioned matrix .
Q = rand(n);
[Q,~] = qr(Q);
smin = -14;%Minimum Singular value 
smax =1; %Maximum Singular value 
d = diag(flip(logspace(smin,smax,n)));
%%

A = U*d*U'; % ill-conidtioned matrix

Cond_numb = cond(A)
%%

b1 = rand(n,1);  % make a right hand side vector

b2 = b1;  
b2(1) = b1(1) + 1e-6;   %change the first element of b2 by a very small value.



x1 = A\b1
x2 = A\b2


Rel_Forward_Error = norm(x2-x1)/norm(x1)

Rel_Backward_Error = norm(b2-b1)/norm(b1)

upper_bound_rel_Forward_Error = Cond_numb*Rel_Backward_Error




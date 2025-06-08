from random import randint
import sys
import time
from math import sqrt

import matplotlib.pyplot as plt


def pgcd(a, b):
	q = a // b
	r = a % b
	if r == 0:
		return b
	return pgcd(b, r)


def pollardsrho(n, f):
	x = randint(0, n)
	y = x

	#print("choix: "+str(x))

	while pgcd(y-x, n) == 1 or pgcd(y-x, n) == n:
		x = f(x, n) % n
		y = f(f(y, n) % n, n) % n

	return pgcd(y-x, n)


def convert(hex):
	s = 0
	n = len(hex)
	for i in range(n):
		match hex[n-i-1]:
			case 'f': 
				s+= 15*16**(i) 
			case 'e':
				s+= 14*16**i
			case 'd':
				s+= 13*16**i
			case 'c':
				s+= 12*16**i
			case 'b':
				s+= 11*16**i
			case 'a':
				s+= 10*16**i 
			case _:
				s+= int(hex[n-i-1])*16**i
	return s


def pow_mod(b, p, n):
	acc = b % n
	out = 1
	while p != 0:
		if p % 2 == 1: 
			out = out*acc % n
		acc = acc*acc % n
		p = p // 2
	return out



def fermat_test(n, k):
	for _ in range(k):
		a = randint(2, n-1)
		
		if pow_mod(a, n-1, n) != 1:
			return False 
	return True


def generate_prime(size_max, size_min, k):
	n = randint(int(size_min), int(size_max))

	if n % 2 == 0:
		n+= 1

	while not fermat_test(n, k):
		n+= 2

	return n


def pollardsrho_time(n, f):
	before = time.time()
	pollardsrho(n, f)
	after = time.time()
	return (after-before)*1000


def brute_force(n):
	for i in range(2, int(sqrt(n))):
		if n % i == 0:
			return i

	return n


def brute_force_time(n):
	before = time.time()
	brute_force(n)
	after = time.time()
	return (after-before)*1000


#print(fermat_test(7, 3))


#print(pollardsrho(116341*117529, lambda x:x**2+1))


# primes = []

# file = open("primes.32b", 'rb')

# for _ in range(1000):
# 	prime = file.read(4)
# 	if not prime:
# 		break
# 	#print(int.from_bytes(prime, "little"))
# 	primes.append(int.from_bytes(prime, "little"))

# #print(file)

# file.close()


# moy = []
# primes_ = []
# primes__ = []

# for i in range(500, 1000):
# 	if i in primes:
# 		moy.append(1)
# 		primes_.append(1)
# 		primes__.append(i)
# 		continue
# 	_sum = 0
# 	for _ in range(100):
# 		if fermat_test(i, 5):
# 			_sum+= 1
# 	moy.append(_sum/100)

# _sum = 0
# for _ in range(100):
# 	if fermat_test(1729, 1):
# 		_sum+= 1

# print(_sum/100)


# plt.plot([i for i in range(500, 1000)], moy, 'ro', marker='.', label="Moyenne sur 100 itérations")
# plt.plot(primes__, primes_, 'ro', color='purple', marker='.', label='Nombres premiers')
# plt.legend()
# plt.show()


from math import ceil,sqrt, exp, log
from random import randint


def crible(B, X):
    T = [i for i in range(X+1)]

    for j in range(2, X+1):
        p = T[j]
        if p > B: 
            continue

        T[j] = 1

        i = 1
        
        while j+i*p <= X:
            if T[j+i*p] % p == 0:
                T[j+i*p]//= p
            i+= 1

    return T


def is_quadratic_residue(p, n):
    if p == 2: 
        return True

    return pow_mod(n, (p-1)/2, p) == 1


def factor_base(B, n):
    T = [2]

    for i in range(3, B+1):
        if fermat_test(i, 10) and is_quadratic_residue(i, n):
            T.append(i)
    return T


def quadratic_sieve(B, M, n, FB):
    Q = lambda x: x**2 - n

    T = [Q(ceil(sqrt(n)) + i) for i in range(M+1)]

    for i in range(M+1):
        if T[i] == 1:
            continue

        for p in FB:
            while T[i] % p == 0 and T[i] != 0:
                T[i]//= p

                k = 1

                while i + k*p <= M:
                    if T[i+k*p] % p == 0:
                        T[i+k*p]//= p
                    k+= 1

    return T


def filter_sieve(B, M, n, FB):
    T = quadratic_sieve(B, M, n, FB)

    a = ceil(sqrt(n))

    Q = lambda x: x**2 - n

    res = []

    #for i in range(M+1):
    #    if T[i] == 1:
    #        res.append((a+i, Q(a+i)))

    return res


def print_mat(A):
    for i in range(len(A)):
        print(A[i])


def add_list(l1, l2):
    res = []
    for i in range(len(l1)):
        res.append((l1[i] + l2[i]) % 2)
    return res

def empty_list(l):
    for i in range(len(l)):
        if l[i] != 0:
            return False
    return True


def calc_ker(T, B, n):
    FB = factor_base(B, n)

    A = []

    for i in range(len(T)):
        line = []

        for j in range(len(FB)):
            a = 0
            _, k = T[i]

            while k % FB[j] == 0:
                a+= 1
                k//= FB[j]

            line.append(a % 2)
        A.append(line)


    nc = len(FB)
    nl = len(A)

    op_mat = []

    for i in range(nl):
        op_mat.append([0]*i+[1]+[0]*(nl-i-1))

    for j in range(nc):
        i = j

        while i < nl and A[i][j] == 0:
            i+= 1

        if i == nl:
            continue

        A[i], A[j] = A[j], A[i]
        op_mat[i], op_mat[j] = op_mat[j], op_mat[i]

        for k in range(i+1, nl):
            if A[k][j] == 0:
                continue
            for q in range(j, nc):
                A[k][q] = (A[k][q] + A[j][q]) % 2
            op_mat[k] = add_list(op_mat[k], op_mat[j])

    ker = []

    for i in range(len(A)):
        if empty_list(A[i]):
            ker.append(op_mat[i])

    return ker


def L(n):
    return exp(sqrt(log(n)*log(log(n))))


def pgcd(a, b):
    q = a // b
    r = a % b
    if r == 0:
        return b
    return pgcd(b, r)


def calc_factor(n, FB):

    B = ceil((L(n))**(1/sqrt(2)))

    #print(B)

    M = ceil((L(n))**(sqrt(2)))

    T = filter_sieve(B, M, n, FB)

    # ker = calc_ker(T, B, n)

    # FB = factor_base(B, n)

    # s1 = []
    # s2 = []

    # for i in range(len(ker)):
    #     u = 1
    #     v = 1

    #     for j in range(len(ker[i])):
    #         if ker[i][j]:
    #             u*= T[j][1]
    #             v*= T[j][0]

    #     for p in FB:
    #         if u % p == 0:
    #             u//= p

    #     s1.append(u)
    #     s2.append(v)

    #print(s1)
    #print(s2)

    #for i in range(len(s1)):
    #    print((s1[i]**2) % n)
    #    print((s2[i]**2) % n)

    # candidates = []

    # for i in range(len(s1)):
    #     candidates.append(pgcd(n, s2[i]-s1[i]))
    #     candidates.append(pgcd(n, s1[i]+s2[i]))

    # return candidates


def crible_time(n):
    FB = factor_base(ceil((L(n))**(1/sqrt(2))), n)
    before = time.time()
    calc_factor(n, FB)
    after = time.time()
    return (after-before)*1000


y = []
x = []
z = []
t = []
h = []
u = []

for i in range(5, 18):
	for j in range(10):
		sum1 = 0

		p = generate_prime(2**(i-1)+(2**i-2**(i-1))/10*(j+1), 2**(i-1)+(2**i-2**(i-1))/10*j, 25)
		q = generate_prime(2**(i-1)+(2**i-2**(i-1))/10*(j+1), 2**(i-1)+(2**i-2**(i-1))/10*j, 25)
		print(p*q)
		x.append(p*q)

		z.append((p*q)**(1/4))
		h.append((p*q)**(1/2))

		print("i "+str(i))
		print("j "+str(j))

		for _ in range(10):
			done = False

			print('tries')

			while not done:
				try:
					sum1+= pollardsrho_time(p*q, lambda x, n: pow_mod(x, 2, n) + 1)
					done = True
				except KeyboardInterrupt:
					done = False
		
		y.append(sum1/10)
		t.append(brute_force_time(p*q))
		u.append(L(p*q))


amp_sumz = 0
amp_sumh = 0

for i in range(1, len(z)):
	amp_sumz+= y[i]/z[i]*(x[i]-x[i-1])
	amp_sumh+= t[i]/h[i]*(x[i]-x[i-1])

amp_moyz = amp_sumz/(x[len(x)-1]-x[0])
amp_moyh = amp_sumh/(x[len(x)-1]-x[0])

amp_u = y[len(x)-1]/u[len(x)-1]

print(amp_moyh)


for i in range(len(z)):
    z[i] = amp_moyz*z[i]
    h[i] = amp_moyh*h[i]
    u[i] = amp_u*u[i]



plt.plot(x, y, 'ro', label='E(pollardsrho)')
plt.plot(x, z, color = 'blue', label='amp*n^(1/4)')
plt.plot(x, t, 'ro', color = 'green', label='brute force')
plt.plot(x, h, color = 'purple', label='amp*n^(1/2)')
plt.plot(x, u)
plt.xlabel("n")
plt.legend()
plt.show()

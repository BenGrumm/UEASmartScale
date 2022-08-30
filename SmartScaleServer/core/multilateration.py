import numpy as np

"""
Calculate the d value (http://inside.mines.edu/~whereman/papers/Murphy-Hereman-Trilateration-1995.pdf p10) 
by taking the difference between co-ordinate locations squaring them and adding them
(not taking square root as the calculation needs the square of d)
"""
def calculateDSquared(iVal, jVal):
	return (iVal[0] - jVal[0]) ** 2 + (iVal[1] - jVal[1]) ** 2 + (iVal[2] - jVal[2]) ** 2

"""
Calculate the b value (http://inside.mines.edu/~whereman/papers/Murphy-Hereman-Trilateration-1995.pdf p10)
by taking the difference between the position radiis and adding the square of the d value returning half of 
the sum of the two values
"""
def calculateB(nVal, oneVal):
	return 0.5 * ((oneVal[3] ** 2) - (nVal[3] ** 2) + calculateDSquared(nVal, oneVal))

"""
Calculate the A matrix (http://inside.mines.edu/~whereman/papers/Murphy-Hereman-Trilateration-1995.pdf p11)
Convert positions to a matrix of the x, y and z values using the 1st location as an origin by taking the difference
"""
def getAFromPositions(positions):
	arr = np.empty([len(positions) - 1, 3])

	for i in range(1, len(positions)):
		arr[i-1][0] = positions[i][0] - positions[0][0]
		arr[i-1][1] = positions[i][1] - positions[0][1]
		arr[i-1][2] = positions[i][2] - positions[0][2]

	return arr

"""
Calculate the b vector (http://inside.mines.edu/~whereman/papers/Murphy-Hereman-Trilateration-1995.pdf p11)
create 1 dimensional matrix and use b value
"""
def getbFromPositions(positions):
	arr = np.empty([len(positions) - 1, 1])
	
	for i in range(1, len(positions)):
		arr[i-1][0] = calculateB(positions[i], positions[0])

	return arr

"""
http://inside.mines.edu/~whereman/papers/Murphy-Hereman-Trilateration-1995.pdf
Solve for XYZ using least square algorithm by first transorming to form Ax=B
then solve for x which is (X, Y, Z) - (X1, Y1, Z1) -> (1st item in the array)

arrayOfTuples should be in form [(X, Y, Z, radius)] minimum of 4 positions needed
"""
def getXYZFromPositionsAndDistances(arrayOfTuples):
	arr = np.array(arrayOfTuples)

	aArr = getAFromPositions(arr)
	bArr = getbFromPositions(arr)

	coordinates = np.linalg.lstsq(aArr, bArr, rcond=None)[0]
	# Add "origin" (first element) to return result to find actual location
	coordinates[0] = coordinates[0] + arr[0][0]
	coordinates[1] = coordinates[1] + arr[0][1]
	coordinates[2] = coordinates[2] + arr[0][2]
	
	return coordinates

if __name__ == '__main__':
	
    # Answer should be 3, 3, 1
	arrayOfTuples = [(0, 1, 0, 3.741657),  
		   (0, 10, 0, 7.381146),  
		   (10, 0, 1, 7.715773),  
		   (10, 10, 0, 9.449874),]

	arr = np.array(arrayOfTuples)

	A = getAFromPositions(arr)
	b = getbFromPositions(arr)

	solution = np.linalg.solve(A, b)

	print("Solve linear = ", end='')
	print(solution)

	print("Solve least square = ", end='')
	print(getXYZFromPositionsAndDistances(arrayOfTuples))

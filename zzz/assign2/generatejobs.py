from random import randint
arrival_time = 0
prior = 1
with open("jobs","w") as file:
	for i in range(30):
		line = str(i)
		if randint(1,2)==2 or prior>20:
			arrival_time += randint(1,50)
			prior = randint(1,20)
			line += " " + str(prior) + " " + str(arrival_time)
		else:
			prior += randint(0,20)
			line += " " + str(prior) + " " + str(arrival_time)

		io = randint(0,10)
		if randint(1,10)<4:
			# io intensive
			ioupper = 60
			iolower = 10
			cpuupper = 30
			cpulower = 5
			for _ in range(io):
				line += " " + str(randint(cpulower,cpuupper))
				line += " " + str(randint(iolower,ioupper))
			line += " " + str(randint(cpulower,cpuupper))
		else:
			# cpu intensive
			ioupper = 30
			iolower = 5
			cpuupper = 60
			cpulower = 10
			for _ in range(io):
				line += " " + str(randint(cpulower,cpuupper))
				line += " " + str(randint(iolower,ioupper))
			line += " " + str(randint(cpulower,cpuupper))
		line += " -1\n"
		file.write(line)


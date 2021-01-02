import sys
import re
import time
import asyncio
import aiohttp
import json

which_name = ("Goloman", "Hands", "Holiday", "Wilkes", "Welsh")

which_port = {
		"Goloman" : 12070,
		"Hands" : 12071,
		"Holiday" : 12072,
		"Wilkes" : 12073,
		"Welsh" : 12074
}

which_contact = {
		"Goloman" : ["Hands", "Holiday", "Wilkes"],
		"Hands" : ["Goloman", "Wilkes"],
		"Holiday" : ["Welsh", "Wilkes"],
		"Wilkes" : ["Goloman", "Hands", "Holiday"],
		"Welsh" : ["Holiday"]
}

client_locations = {}

async def place_request(info, writer):
	log_file.write("\nGetting client's location\n")
	google_url = "https://maps.googleapis.com/maps/api/place/nearbysearch/json?key={}&location={},{}&radius={}".format("AIzaSyCe6RqX1UvqdtdFLvVTrv8mXJCHXfuto2g", client_locations[info[0]][1][0], client_locations[info[0]][1][1], info[1])
	# TA's code from Week 8 section 1C slides as reference
	async with aiohttp.ClientSession() as session:
		async with session.get(google_url) as response:
			data = await response.json()
			log_file.write("Response recieved from google\n")
			if(int(info[2]) < len(data["results"])):
				data["results"] = data["results"][:info[2]]
			log_file.write("Responding to client WHATSAT request\n")
			message = "AT " + server_name + (" +", " ")[client_locations[info[0]][0] < 0] + str(client_locations[info[0]][0]) + " " + info[0] + (" +", " ")[client_locations[info[0]][1][0] < 0] + str(client_locations[info[0]][1][0]) + ("+", "")[client_locations[info[0]][1][1] < 0] + str(client_locations[info[0]][1][1]) + " " + str(client_locations[info[0]][2]) + "\n" + json.dumps(data, indent=3, separators=(","," : "))
			writer.write(message.encode())
			log_file.write("AT response:\n{}\n".format(message))
	

# possible use of https://docs.python.org/3/library/asyncio-stream.html as reference
async def update_and_contact(data):
	log_file.write("\nChecking local client data\n")
	client_time = time.time() - float(data[2])
	coords = re.split(r"[\s]", re.sub(r"([\d])([^\d\.])", r"\1 \2", data[1]))
	if((data[0] not in client_locations) or (client_locations[data[0]][0] >= client_time)):
		client_locations[data[0]] = (client_time, list(map(float, coords)), float(data[2]))
		log_file.write("Updating local client data of {}: {}\n".format(data[0], client_locations[data[0]]))
	else:
		log_file.write("No update; keeping data of {}: {}\n".format(data[0], client_locations[data[0]]))
		log_file.write("Ignoring data: {}\n".format((client_time, list(map(float, coords)), float(data[2]))))
	already_traversed = data[3:]
	traverse_string = (" " + (" ".join(already_traversed)))
	if(traverse_string == " "):
		traverse_string += server_name
	elif(server_name not in already_traversed):
		traverse_string += (" " + server_name)
	for x in which_contact[server_name]:
		if x not in already_traversed:
			traverse_string += (" " + x)
	message = "AT " + str(data[0]) + " " + str(data[1]) + " " + str(data[2]) + traverse_string
	for contact in which_contact[server_name]:
		if(contact not in already_traversed):
			try:
				log_file.write("Trying to contact {}\n".format(contact))
				(reader, writer) = await asyncio.open_connection(host='127.0.0.1', port=which_port[contact])
				log_file.write("Connection with {} established\n".format(contact))
				log_file.write("AT response to {}: {}\n".format(contact, message))
				writer.write(message.encode())
				log_file.write("AT response to {} successful\n".format(contact))
				await writer.drain()
				writer.close()
			except:
				log_file.write("Cannot connect to {}\n".format(contact))
				continue

async def client_response(info, writer):
	time_diff = time.time() - float(info[2])
	response = "AT " + server_name + " " + ("+", "")[time_diff < 0] + str(time_diff) + " " + str(info[0]) + " " + str(info[1]) + " " + str(info[2])
	log_file.write("\nAT response to client: {}\n".format(response))
	writer.write(response.encode())
	log_file.write("AT response to client successful\n")
	await update_and_contact(info)

async def which_action(reader, writer):
	try:
		data = await reader.read()
		request = data.decode()
		log_file.write("\nData received: {}\n".format(request))
		to_parse = re.split(r"[\s]+", request.rstrip())
		if((to_parse[0] == "IAMAT") and (len(to_parse) == 4)):
			await client_response(to_parse[1:4], writer)
		elif((to_parse[0] == "WHATSAT") and (len(to_parse) == 4)):
			if(to_parse[1] not in client_locations):
				writer.write(("? " + request).encode())
				log_file.write("Client data not found")
			else:
				await place_request(to_parse[1:4], writer)
		elif((to_parse[0] == "AT") and (len(to_parse) >= 4)):
			await update_and_contact(to_parse[1:])
		else:
			writer.write(("? " + request).encode())
	finally:
		await writer.drain()
		writer.close()

async def main():
	if(len(sys.argv) != 2):
		print("Invalid number of arguments")
		sys.exit(1)
	if(sys.argv[1] not in which_name):
		print("Invalid server")
		sys.exit(1)
	global server_name
	server_name = sys.argv[1]
	global log_file
	log_file = open((server_name + ".log"),'w')
	server = await asyncio.start_server(which_action, host='127.0.0.1', port=which_port[server_name])
	log_file.write("Server initialized\n")
	try:
		await server.serve_forever()
	finally:
		log_file.write("\nServer closing\n")
		log_file.close()

if __name__ == '__main__':
	asyncio.run(main())




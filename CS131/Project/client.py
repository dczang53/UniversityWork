import asyncio

async def main():
	reader, writer = await asyncio.open_connection('127.0.0.1', 12073)
	writer.write("John\n".encode())
	data = await reader.readline()
	print('Received: {}'.format(data.decode()))
	writer.close()

if __name__ == '__main__':
	asyncio.run(main())

#JUST USE "nc localhost <port> -q 1"
#and concate EOF with ctrl-D

all: file_manager file_client run
file_manager: file_manager.c
		 gcc file_manager.c -o file_manager -l pthread
file_client: file_client.c
		 gcc file_client.c -o file_client -lreadline
run:
	./file_manager
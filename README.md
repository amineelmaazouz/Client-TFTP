# Client-TFTP
C’est quoi un protocol TFTP ? Le TFTP (protocole de transfert de fichiers trivial) est un système de transfert de fichiers simple, qui ne nécessite pas d'authentification pour être utilisé. Il est conçu pour les applications qui ne requièrent pas les fonctionnalités avancées offertes par le protocole FTP (transfert de fichiers).

# Utilisation des arguments passés à la ligne de commande des programmes gettftp et puttftp pour obtenir les informations de requêtes (serveur et fichier)
On récupère d'abord les informations sur la requête (nom de serveur et de fichier) sur la commande des programmes gettftp et puttftp. Pour cela nous récupérons les informations passés en arguments, avec le nom du serveur passé en argv[1] et le nom du fichier passé en argv[2].

# Appel à getaddrinfo pour obtenir l’adresse du serveur
Le code de cette partie est dans le fichier “getaddrinfo.c”
Dans cette partie nous avons créé un programme simple qui prend le nom d'un serveur en argument et utilise la fonction getaddrinfo() pour obtenir l'adresse IP du serveur. La fonction getaddrinfo() est utilisée pour obtenir des informations d'adresse pour un nom d'hôte ou une adresse IP donnés.

![1](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/1a05973b-cbe5-49dd-a0f8-ba87a130abba)

# Réservation d’un socket de connexion vers le serveur
Dans cette partie nous avons établi une connexion de socket à un serveur à une adresse spécifiée en utilisant la fonction getaddrinfo().
![2](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/7988eaf1-eef7-4330-abaa-8bda12ffca05)

# Gettftp
Dans cette partie nous avons commencé par la création de la fonction gettftp qui cherche un document dans le serveur, et crée une copie de celui-ci dans notre repertoire.
D’abord nous avons commencé par l’importation du serveur dans notre machine virtuelle et de lancer l’objet ./go.sh

![3](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/c3c6654f-b650-4abf-9712-e909f74dd436)

En suivant les spécifications du protocole TFTP, nous avons construit un paquet RRQ (Read Request), qui va assurer la conformité avec le format requis.
Ce paquet a été ensuite envoyé au serveur grâce à la fonction sendto.

![4](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/3e007ce8-2ced-4d35-9330-b65860af6d45)

Notre programme procède à la réception du fichier (paquet de données) constitué d'un seul bloc (DAT) ainsi que la gestion de son acquittement (ACK). 
Le contenu du fichier reçu a été extrait et enregistré localement. En conclusion, un accusé de réception (ACK) a été renvoyé au serveur.
Nous trouvons ainsi les fichiers dans notre répertoire :
![5](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/229c3ba9-1259-4b4b-bcaa-76a019c1c537)

Dans ce screen nous générons le fichier zeros256 qui est un fichier texte ici alors réellement ce n’est pas le cas.
Nous modifions la fonction getFile qui est responsable de la récupération d'un fichier depuis le serveur TFTP et construit une RRQ au format TFTP.

![6](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/2657cdc9-f1fb-44e8-ab95-7fd2c8266988)

![7](https://github.com/amineelmaazouz/Client-TFTP/assets/78564470/b8950659-c39a-44df-a63e-113ce4d9660a)

Maintenant nous générons bien le fichier zeros256 tel qu’il est réellement.

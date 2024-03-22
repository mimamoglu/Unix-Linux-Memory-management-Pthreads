# Unix-Linux-Memory-management-Pthreads
The program simulates memory management. The scheduler divides file content into 512-byte pages, managing main memory. Clients send file content and names to the scheduler. Clients can request specific pages interactively, and the scheduler provides the requested content.

--C, Unix/Linux, Memory management, Pthreads--

Bir Scheduler programı ve istemci programları kullanarak bir hafıza yönetimi sistemini simüle eder. Scheduler, ana belleği yönetip istemcilerden gelen dosya içeriğini 512byte lık page lere ayırır. İstemci girdi metin dosyalarını okur, ardından okuduğu dosyanın içeriğini ve kendi istemci ismini schedulera gönderir. İstemci etkileşimli olarak belirli sayfaları talep edebilir ve Scheduler talep edilen sayfa içeriğini sağlar.

Dosyalar:

scheduler.c: Bu, hafıza tahsisini yöneten ve istemci taleplerine yanıt veren Scheduler programıdır.

user_process.c: Bu, girdi metin dosyalarını okuyan, Scheduler ile etkileşime geçen ve belirli sayfaları talep eden istemci işlem programıdır.

Kullanım:

Scheduler ve İstemci işlem programlarını ayrı ayrı bir C derleyicisi kullanarak derleyin, örneğin gcc:

gcc scheduler.c -o scheduler -lm
gcc user_process.c -o user_process

Scheduler programını bir terminalde çalıştırın:

./scheduler
İstemci işlem programlarını ayrı terminallerde çalıştırın, işlem adını ve simülasyonu yapmak için metin dosyasının adını sağlayarak:

./user_process <İstemci adı> <Dosya adı>

Örnek:

Terminal 1 (Scheduler):

Scheduler başlatıldı.

Terminal 2 (İstemci İşlemi 1):
Erişmek istediğiniz sayfa numarasını girin (-1 işlemi sonlandırmak için): 3
İstenen sayfa :
This is page 3 content.

Terminal 3 (İstemci İşlemi 2):
Erişmek istediğiniz sayfa numarasını girin (-1 işlemi sonlandırmak için): 2
İstenen sayfa :
This is page 2 content.

Scheduler, İstemci işlem taleplerine dayalı olarak sayfalar için çerçeve tahsis eder.
İstemci, sayfa numarasına göre belirli sayfaları talep edebilir.
Bir istemci işlemi var olmayan bir sayfa talep ederse veya kullanılabilir çerçeve yoksa, istemci sonlanır ve ona ait çerçeveler serbest bırakılır.
Scheduler ve istemci , mesaj kuyruklarını kullanarak iletişim kurar.
İstemci işlemi çalıştırırken <İstemci adı> ve <Dosya adı> yerine uygun değerleri kullanmayı unutmayın.


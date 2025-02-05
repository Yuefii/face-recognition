import cv2

url = 'http://192.168.1.100:81/video'
cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("Gagal mengakses kamera!")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Gagal membaca frame!")
        break

    cv2.imshow("ESP32-CAM Stream", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

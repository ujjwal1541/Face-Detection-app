import cv2
import tkinter as tk
from tkinter import ttk, filedialog
from PIL import Image, ImageTk
import os
from datetime import datetime

class FaceDetectionApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Face Detection App")
        
        # Initialize variables
        self.cap = None
        self.is_running = False
        self.face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
        
        # Create GUI
        self.setup_gui()
        
        # Create faces directory if it doesn't exist
        if not os.path.exists('faces'):
            os.makedirs('faces')
    
    def setup_gui(self):
        # Main frame
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Video display
        self.video_label = ttk.Label(main_frame)
        self.video_label.grid(row=0, column=0, columnspan=3, padx=5, pady=5)
        
        # Controls
        self.start_button = ttk.Button(main_frame, text="Start", command=self.start_camera)
        self.start_button.grid(row=1, column=0, padx=5, pady=5)
        
        self.stop_button = ttk.Button(main_frame, text="Stop", command=self.stop_camera, state=tk.DISABLED)
        self.stop_button.grid(row=1, column=1, padx=5, pady=5)
        
        self.file_button = ttk.Button(main_frame, text="Open Video File", command=self.open_file)
        self.file_button.grid(row=1, column=2, padx=5, pady=5)
        
        # Face count label
        self.face_count_label = ttk.Label(main_frame, text="Faces detected: 0")
        self.face_count_label.grid(row=2, column=0, columnspan=3, pady=5)
    
    def start_camera(self):
        if self.cap is None:
            self.cap = cv2.VideoCapture(0)
        self.is_running = True
        self.start_button.config(state=tk.DISABLED)
        self.stop_button.config(state=tk.NORMAL)
        self.file_button.config(state=tk.DISABLED)
        self.update_frame()
    
    def stop_camera(self):
        self.is_running = False
        if self.cap is not None:
            self.cap.release()
            self.cap = None
        self.start_button.config(state=tk.NORMAL)
        self.stop_button.config(state=tk.DISABLED)
        self.file_button.config(state=tk.NORMAL)
        self.video_label.config(image='')
    
    def open_file(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("Video files", "*.mp4 *.avi *.mkv")]
        )
        if file_path:
            if self.cap is not None:
                self.cap.release()
            self.cap = cv2.VideoCapture(file_path)
            self.is_running = True
            self.start_button.config(state=tk.DISABLED)
            self.stop_button.config(state=tk.NORMAL)
            self.file_button.config(state=tk.DISABLED)
            self.update_frame()
    
    def update_frame(self):
        if self.is_running and self.cap is not None:
            ret, frame = self.cap.read()
            if ret:
                # Convert to grayscale for face detection
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                
                # Detect faces
                faces = self.face_cascade.detectMultiScale(gray, 1.1, 4)
                
                # Draw rectangles around faces and save them
                for (x, y, w, h) in faces:
                    cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                    
                    # Save face
                    face_img = frame[y:y+h, x:x+w]
                    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
                    cv2.imwrite(f'faces/face_{timestamp}.jpg', face_img)
                
                # Update face count
                self.face_count_label.config(text=f"Faces detected: {len(faces)}")
                
                # Convert frame to PhotoImage
                frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                frame = cv2.resize(frame, (640, 480))
                photo = ImageTk.PhotoImage(image=Image.fromarray(frame))
                
                # Update video label
                self.video_label.config(image=photo)
                self.video_label.image = photo
                
                # Schedule next update
                self.root.after(10, self.update_frame)
            else:
                self.stop_camera()

if __name__ == "__main__":
    root = tk.Tk()
    app = FaceDetectionApp(root)
    root.mainloop() 
# Tower Defense

Game thủ thành 2D viết bằng C++ và SDL3. Người chơi xây tháp, chống lại các đợt quái vật và bảo vệ căn cứ, người chơi có 3 mạng.

## Tính năng

- Bản đồ lưới 15x15 với cỏ, đường đi, căn cứ, nước và dung nham.
- Đường đi của quái được đọc từ các tệp trong `assets/Map/`.
- Ba chế độ: `Normal`, `Endless`.
- Năm loại tháp:
  - `Basic`: tấn công cân bằng.
  - `Frost`: gây sát thương và làm chậm.
  - `Electric`: sát thương điện có thể lan sang mục tiêu khác.
  - `Cannon`: gây sát thương diện rộng.
  - `Poison`: tấn công nhanh ở khoảng cách gần.
- Nhiều loại quái với đặc tính và phần thưởng khác nhau.
- Hệ thống wave, tiền, nâng cấp tháp, điểm số và thống kê trận đấu.
- Hiệu ứng nổ, tia điện, hạt sáng, thanh máu enemy và nhạc nền.
- Các màn hình `Main Menu`, `Level Select`, `Settings`, `Playing`, `Paused` và `Game Over`.

## Yêu cầu

- Windows.
- Trình biên dịch C++ hỗ trợ C++17 trở lên, chẳng hạn MinGW-w64 `g++`.
- SDL3 và SDL3_ttf. Header nằm trong `include/SDL3/`, thư viện liên kết nằm trong `lib/`.
- Giữ nguyên thư mục `assets/` để game tải được hình ảnh, font, âm thanh và dữ liệu bản đồ.

## Build và chạy

Mở PowerShell hoặc Command Prompt tại thư mục gốc của dự án, sau đó chạy:

```bat
build.bat
bin\TowerDefenseGame.exe
```

Lệnh build thủ công tương đương:

```bat
g++ src/main.cpp src/InputSystem.cpp src/LogicSystem.cpp src/RenderSystem.cpp src/MapSystem.cpp src/AssetManager.cpp src/SoundSystem.cpp src/EffectsSystem.cpp src/ScoreManager.cpp -I include -L lib -o bin/TowerDefenseGame -lSDL3 -lSDL3_ttf -lwinmm
```

Chạy file thực thi từ thư mục gốc dự án vì game sử dụng đường dẫn tương đối như `assets/Image/` và `assets/Music/`.

## Cách chơi

1. Khởi động game và chọn chế độ chơi.
2. Chọn chế độ chơi phù hợp: `Normal`, `Endless` hoặc `Challenge`.
3. Chọn loại tháp, sau đó nhấn chuột trái vào ô hợp lệ để xây tháp.
4. Kết hợp các loại tháp để tận dụng sát thương, làm chậm, điện lan và sát thương diện rộng.
5. Tiêu diệt enemy để nhận gold và tiếp tục xây dựng hoặc nâng cấp phòng thủ.
6. Ngăn enemy đi tới căn cứ. Trận đấu kết thúc khi HP căn cứ giảm về 0.

## Điều khiển

| Thao tác | Chức năng |
| --- | --- |
| Chuột trái | Chọn nút, chọn tháp hoặc xây tháp |
| Di chuyển chuột | Xem ô đang được trỏ tới trên bản đồ |
| Các nút trong giao diện | Tạm dừng, đổi tốc độ, mở cài đặt, chọn level hoặc chơi lại |

## Cấu trúc dự án

```text
Tower_Defense/
├── assets/       # Hình ảnh, font, âm thanh và dữ liệu bản đồ
├── bin/          # File thực thi sau khi build
├── include/      # Header của game và SDL3
├── lib/          # Thư viện SDL3, SDL3_ttf
├── src/          # Mã nguồn C++
├── build.bat     # Script build trên Windows
└── README.md
```

Các module chính trong `src/` gồm quản lý asset, input, logic game, bản đồ, render, âm thanh, hiệu ứng và điểm số.
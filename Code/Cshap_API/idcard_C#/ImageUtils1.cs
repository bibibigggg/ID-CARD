using System;
using System.IO;
using idcard_C_;

namespace idcard_C_
{
    public static class ImageUtils
    {
        public static string ConvertImageToBase64(string imagePath)
        {
            var imageBytes = File.ReadAllBytes(imagePath);
            return Convert.ToBase64String(imageBytes);
        }
    }
}

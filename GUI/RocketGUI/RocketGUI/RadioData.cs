using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using XBee;
using System.Diagnostics;
using System.IO;

namespace RocketGUI
{
    public class RadioData
    {
        public int tc1 { get; set; }
        public int pt1 { get; set; }

        public RadioData(int tc1, int pt1)
        {
            this.tc1 = tc1;
            this.pt1 = pt1;
        }
    }
}

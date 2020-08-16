using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.Mvc;

namespace AdrenaWeb.Controllers {
    public class HomeController : Controller {
        public ActionResult Index() {
            return View();
        }
    }
}
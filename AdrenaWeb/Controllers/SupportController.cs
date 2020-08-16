using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Mail;
using System.Web;
using System.Web.Mvc;

namespace AdrenaWeb.Controllers
{
    public class SupportController : Controller
    {
        public ActionResult AdrenaSupport()
        {
            return View();
        }
    }
}
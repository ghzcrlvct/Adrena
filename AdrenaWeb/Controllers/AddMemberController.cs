using Microsoft.SqlServer.Server;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http.Headers;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Web;
using System.Web.Mvc;
using System.Web.UI.WebControls;

namespace AdrenaWeb.Controllers {
    public class AddMemberController : Controller
    {
        [HttpGet]
        public ActionResult AdrenaAddMember()
        {
            return View();
        }

        [HttpPost]
        public ActionResult AdrenaAddMember(string MasterSecret, string Name, string Secret, string Options) {
            string MbFld = Server.MapPath("~/AdrenaMembers");

            if(0 != string.Compare(MasterSecret, AdrenaMemberHelper.SpecialSecret)) {
                ADRENA_MEMBER Mb = new ADRENA_MEMBER();
                if (!AdrenaMemberHelper.AdrenaMemberFindWithSecret(out Mb, MbFld, MasterSecret)) {
                    ViewBag.LastError = "INVALID SECRET!";

                    return View();
                }
            }

            if (!string.IsNullOrEmpty(Secret)) {
                if (Options.ToUpper().Contains("~BAN")) {
                    if(!AdrenaMemberHelper.AdrenaMemberBan(MbFld, Secret))
                        ViewBag.LastError = "COULD NOT BAN USER!";

                    string LgPth = Server.MapPath("~/AdrenaLog");
                    AdrenaMemberHelper.AdrenaMemberStoreBanLog(LgPth, Secret);
                }

                ViewBag.Success = true;

                return View();
            }

            if (string.IsNullOrEmpty(Name)) {
                ViewBag.Success = true;

                return View();
            }

            string NewMbScrt;
            if(!AdrenaMemberHelper.AdrenaMemberGenerateSecret(out NewMbScrt)) {
                ViewBag.LastError = "CANNOT GENERATE SECRET!";

                return View();
            }

            bool SingleUse = false;
            if (Options.ToUpper().Contains("~SU"))
                SingleUse = true;

            if(!AdrenaMemberHelper.AdrenaMemberAdd(Name, MbFld, NewMbScrt, SingleUse)) {
                ViewBag.LastError = "CANNOT ADD MEMBER!";

                return View();
            }

            ViewBag.Secret = NewMbScrt;

            return View();
        }
    }
}
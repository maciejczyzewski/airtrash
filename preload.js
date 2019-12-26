var path = require("path");
const workerFarm = require("worker-farm"),
      {dialog} = require("electron").remote;
var mustache = require("mustache"),
    NativeExtension = require("bindings")("airtrash");
const service_push = workerFarm(require.resolve("./push")),
      service_pull = workerFarm(require.resolve("./pull"));

var PORT = 9000, REFRESH_MS = 3000, LOCAL_IP = NativeExtension.get_local_ip();

/// API ////////////////////////////////////////////////////////////////////////

function api_push_file(local_path) {
  data = {address : LOCAL_IP + ":" + PORT, path : local_path},
  service_push(data,
               function(err, output) {
                 new Notification(
                     "Transmission Closed!",
                     {body : output.path + " from " + output.address})
               }),
  ++PORT,
  new Notification("File Published!",
                   {body : data.path + " from " + data.address}),
  gui_add_element(document.getElementById("files"), data),
  gui_add_listeners("download", api_pull_file)
}

function api_pull_file() {
  data = {
    address : this.getAttribute("address"),
    path_base :
        path.parse(this.getAttribute("path").replace(/[^\x00-\x7F]/g, "")).base
  },
  dialog.showSaveDialog({defaultPath : data.path_base})
      .then(output => (item = output.filePath,
                       void 0 === item || "" === item
                           ? void alert("Download procedure cancelled!")
                           : void service_pull(
                                 {address : data.address, path : item},
                                 function() {
                                   new Notification(
                                       "Download Complete!",
                                       {body : item + " from " + data.address})
                                 })))
      .catch(err => {alert(err)})
}

function api_scan() {
  var protocol = NativeExtension.scan().split("|").filter(Boolean),
      DOM = document.getElementById("files");
  DOM.innerHTML = "", protocol.forEach(function(item) {
    label = item.split("&"),
    gui_add_element(DOM, {address : label[0], path : label[1]})
  }),
  0 < protocol.length && gui_add_listeners("download", api_pull_file)
}

/// GUI ////////////////////////////////////////////////////////////////////////

function gui_drag_and_drop(DOM) {
  classes = "mt-auto p-4 ",
  DOM.ondragover = () => (DOM.setAttribute("class", classes + "bg-danger"), !1),
  DOM.ondragleave = () =>
      (DOM.setAttribute("class", classes + "bg-warning"), !1),
  DOM.ondragend = () =>
      (DOM.setAttribute("class", classes + "bg-highlight"), !1),
  DOM.ondrop = e => {
    e.preventDefault();
    for (let item of e.dataTransfer.files)
      api_push_file(item.path);
    return DOM.setAttribute("class", classes + "bg-highlight"), !1
  }
}

function gui_add_element(DOM, data) {
  data.bg = "", data.address.includes(LOCAL_IP) && (data.bg = "bg-success-own");
  DOM.innerHTML += mustache.to_html(`<tr class='{{bg}}'>
      <td class='long-address'><b>{{address}}</b></td>
      <td class='long-path'>{{path}}</td>
      <td><a class='download btn badge btn-primary btn-sm text-white'
        address='{{address}}' path='{{path}}'>download</a></td>
    </tr>`,
                                    data);
}

function gui_add_listeners(classname, func) {
  for (var c = document.getElementsByClassName(classname), d = 0; d < c.length;
       d++)
    c[d].addEventListener("click", func, !1)
}

////////////////////////////////////////////////////////////////////////////////

window.addEventListener("DOMContentLoaded", () => {
  gui_drag_and_drop(document.getElementById("dropzone")),
  document.querySelector("#manual").addEventListener(
      "click",
      function() {
        dialog.showOpenDialog({properties : [ "openFile", "multiSelections" ]})
            .then(output => {
              for (let item of output.filePaths)
                api_push_file(item)
            })
      }),
  setInterval(function() { api_scan() }, REFRESH_MS)
});

package main

import (
	"launchpad.net/go-unityscopes/v1"
	"log"
)

func main() {
	scope := &Scope{}
	if err := scopes.Run(scope); err != nil {
		log.Fatalln(err)
	}
}

type Scope struct {
	base *scopes.ScopeBase
}

func (sc *Scope) SetScopeBase(base *scopes.ScopeBase) {
	sc.base = base
}

const categoryTemplate = `
{
  "schema-version": 1,
  "template": {
    "category-layout": "grid",
    "card-layout": "horizontal",
    "card-size": "medium"
  },
  "components": {
    "title": "title",
    "art" : {
      "field": "art"
    },
    "subtitle": "subtitle"
  }
}
`

func (sc *Scope) Search(query *scopes.CannedQuery, metadata *scopes.SearchMetadata, reply *scopes.SearchReply, cancelled <-chan bool) error {
	qs := query.QueryString()

	cat := reply.RegisterCategory("results", "Results", "", categoryTemplate)
	result := scopes.NewCategorisedResult(cat)
	result.SetURI("http://example.com/")
	result.SetTitle("Search for " + qs)
	result.SetArt("/.../foo.png")
	result.Set("subtitle", "Result subtitle")
	result.Set("description", "Result description")
	if err := reply.Push(result); err != nil {
		return err
	}
	return nil
}

func (sc *Scope) Preview(result *scopes.Result, metadata *scopes.ActionMetadata, reply *scopes.PreviewReply, cancelled <-chan bool) error {
	// Define the header section
	header := scopes.NewPreviewWidget("header", "header")
	header.AddAttributeMapping("title", "title")
	header.AddAttributeMapping("subtitle", "subtitle")

	// Define the image section
	image := scopes.NewPreviewWidget("image", "image")
	image.AddAttributeMapping("source", "art")

	// Define the summary section
	description := scopes.NewPreviewWidget("summary", "text")
	description.AddAttributeMapping("text", "description")

	return reply.PushWidgets(header, image, description)
}

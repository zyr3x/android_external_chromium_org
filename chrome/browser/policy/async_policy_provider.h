// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_POLICY_ASYNC_POLICY_PROVIDER_H_
#define CHROME_BROWSER_POLICY_ASYNC_POLICY_PROVIDER_H_

#include "base/cancelable_callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/non_thread_safe.h"
#include "chrome/browser/policy/configuration_policy_provider.h"

namespace base {
class MessageLoopProxy;
}

namespace policy {

class AsyncPolicyLoader;
class PolicyBundle;

// A policy provider that loads its policies asynchronously on the FILE thread.
// Platform-specific providers are created by passing an implementation of
// AsyncPolicyLoader to a new AsyncPolicyProvider.
class AsyncPolicyProvider : public ConfigurationPolicyProvider,
                            public base::NonThreadSafe {
 public:
  explicit AsyncPolicyProvider(scoped_ptr<AsyncPolicyLoader> loader);
  virtual ~AsyncPolicyProvider();

  // ConfigurationPolicyProvider implementation.
  virtual void Init() OVERRIDE;
  virtual void Shutdown() OVERRIDE;
  virtual void RefreshPolicies() OVERRIDE;
  virtual void RegisterPolicyDomain(
      scoped_refptr<const PolicyDomainDescriptor> descriptor) OVERRIDE;

 private:
  // Helper for RefreshPolicies().
  void ReloadAfterRefreshSync();

  // Invoked with the latest bundle loaded by the |loader_|.
  void OnLoaderReloaded(scoped_ptr<PolicyBundle> bundle);

  // Callback passed to the loader that it uses to pass back the current policy
  // bundle to the provider. This is invoked on the FILE thread and forwards
  // to OnLoaderReloaded() on the loop that owns the provider, if |weak_this| is
  // still valid.
  static void LoaderUpdateCallback(scoped_refptr<base::MessageLoopProxy> loop,
                                   base::WeakPtr<AsyncPolicyProvider> weak_this,
                                   scoped_ptr<PolicyBundle> bundle);

  // The |loader_| that does the platform-specific policy loading. It lives
  // on the FILE thread but is owned by |this|.
  AsyncPolicyLoader* loader_;

  // Used to get a WeakPtr to |this| for the update callback given to the
  // loader.
  base::WeakPtrFactory<AsyncPolicyProvider> weak_factory_;

  // Callback used to synchronize RefreshPolicies() calls with the FILE thread.
  // See the implementation for the details.
  base::CancelableClosure refresh_callback_;

  DISALLOW_COPY_AND_ASSIGN(AsyncPolicyProvider);
};

}  // namespace policy

#endif  // CHROME_BROWSER_POLICY_ASYNC_POLICY_PROVIDER_H_
